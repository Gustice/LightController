/**
 * @file MyWeb.cpp
 * @author Gustice
 * @brief Implementation of Web-API
 * @details This Function initializes a web server with different API-Routes to 
 *  - Request Files
 *  - Request Data
 *  - Set Data
 * @version 0.1
 * @date 2021-03-04
 * 
 * @copyright Copyright (c) 2021
 */

#include "includes/MyWeb.h"

#include "ProcessHandler.h"
#include "cJSON.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include "esp_vfs.h"
#include "nvs_flash.h"
#include "protocol_common.h"
#include <esp_event.h>
#include <esp_http_server.h>
#include <stdio.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <fcntl.h>
#include <nvs_flash.h>
#include <sstream>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_spiffs.h"

#include "ParamReader.h"
#include "SoftAp.h"
#include "WebUtils.h"


static const char *cModTag = "WEB";
static const char *webBasePath = CONFIG_WEB_MOUNT_POINT;
static const char *setupBasePath = CONFIG_SETUP_MOUNT_POINT;
#define FILE_PATH_MAX   (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)
#define MAX_FILE_SIZE   (200*1024) // 200 KB
#define MAX_FILE_SIZE_STR "200KB"

Color_t color;
uint8_t intensity;
SemaphoreHandle_t xNewLedWebCommand;

struct file_server_data {
    /* Base path of file storage */
    char base_path[ESP_VFS_PATH_MAX + 1];

    /* Scratch buffer for temporary storage during file transfer */
    char scratch[SCRATCH_BUFSIZE];
};


static const char* get_path_from_uri(char *dest, const char *base_path, const char *uri, size_t destsize)
{
    const size_t base_pathlen = strlen(base_path);
    size_t pathlen = strlen(uri);

    const char *quest = strchr(uri, '?');
    if (quest) {
        pathlen = MIN(pathlen, quest - uri);
    }
    const char *hash = strchr(uri, '#');
    if (hash) {
        pathlen = MIN(pathlen, hash - uri);
    }

    if (base_pathlen + pathlen + 1 > destsize) {
        /* Full path string won't fit into destination buffer */
        return NULL;
    }

    /* Construct full path (base + path) */
    sprintf(dest, "%s/%s", base_path, uri );

    /* Return pointer to path, skipping the base */
    return dest + base_pathlen + 1;
}

/* Handler to download a file kept on the server */
static esp_err_t download_get_handler(httpd_req_t *req)
{
    char filepath[FILE_PATH_MAX];
    FILE *fd = NULL;

    const char *filename = get_path_from_uri(filepath, ((struct file_server_data *)req->user_ctx)->base_path,
                                             req->uri + sizeof("/config"), sizeof(filepath));
    if (!filename) {
        ESP_LOGE(cModTag, "Filename is too long");
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
        return ESP_FAIL;
    }

    /* If name has trailing '/', respond with directory contents */
    if (filename[strlen(filename) - 1] == '/') {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File does not exist");
        return ESP_FAIL;
    }

    fd = fopen(filepath, "r");
    if (!fd) {
        ESP_LOGE(cModTag, "Failed to read existing file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    ESP_LOGI(cModTag, "Sending file : %s ...", filename);
    set_content_type_from_file(req, filename);

    /* Retrieve the pointer to scratch buffer for temporary storage */
    char *chunk = ((struct file_server_data *)req->user_ctx)->scratch;
    size_t chunksize;
    do {
        /* Read file in chunks into the scratch buffer */
        chunksize = fread(chunk, 1, SCRATCH_BUFSIZE, fd);

        if (chunksize > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK) {
                fclose(fd);
                ESP_LOGE(cModTag, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
               return ESP_FAIL;
           }
        }

        /* Keep looping till the whole file is sent */
    } while (chunksize != 0);

    /* Close file after sending complete */
    fclose(fd);
    ESP_LOGI(cModTag, "File sending complete");

    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/* Handler to upload a file onto the server */
static esp_err_t upload_post_handler(httpd_req_t *req)
{
    char filepath[FILE_PATH_MAX];
    FILE *fd = NULL;
    struct stat file_stat;

    /* Skip leading "/upload" from URI to get filename */
    /* Note sizeof() counts NULL termination hence the -1 */
    const char *filename = get_path_from_uri(filepath, ((struct file_server_data *)req->user_ctx)->base_path,
                                             req->uri + sizeof("/uploadconfig") - 1, sizeof(filepath));
    if (!filename) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
        return ESP_FAIL;
    }

    /* Filename cannot have a trailing '/' */
    if (filename[strlen(filename) - 1] == '/') {
        ESP_LOGE(cModTag, "Invalid filename : %s", filename);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Invalid filename");
        return ESP_FAIL;
    }

    if (stat(filepath, &file_stat) == 0) {
        ESP_LOGE(cModTag, "File already exists : %s", filepath);
        /* Respond with 400 Bad Request */
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "File already exists");
        return ESP_FAIL;
    }

    /* File cannot be larger than a limit */
    if (req->content_len > MAX_FILE_SIZE) {
        ESP_LOGE(cModTag, "File too large : %d bytes", req->content_len);
        /* Respond with 400 Bad Request */
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                            "File size must be less than "
                            MAX_FILE_SIZE_STR "!");
        /* Return failure to close underlying connection else the
         * incoming file content will keep the socket busy */
        return ESP_FAIL;
    }

    fd = fopen(filepath, "w");
    if (!fd) {
        ESP_LOGE(cModTag, "Failed to create file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to create file");
        return ESP_FAIL;
    }

    ESP_LOGI(cModTag, "Receiving file : %s...", filename);

    /* Retrieve the pointer to scratch buffer for temporary storage */
    char *buf = ((struct file_server_data *)req->user_ctx)->scratch;
    int received;

    /* Content length of the request gives
     * the size of the file being uploaded */
    int remaining = req->content_len;

    while (remaining > 0) {

        ESP_LOGI(cModTag, "Remaining size : %d", remaining);
        /* Receive the file part by part into a buffer */
        if ((received = httpd_req_recv(req, buf, MIN(remaining, SCRATCH_BUFSIZE))) <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry if timeout occurred */
                continue;
            }

            /* In case of unrecoverable error,
             * close and delete the unfinished file*/
            fclose(fd);
            unlink(filepath);

            ESP_LOGE(cModTag, "File reception failed!");
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file");
            return ESP_FAIL;
        }

        /* Write buffer content to file on storage */
        if (received && (received != fwrite(buf, 1, received, fd))) {
            /* Couldn't write everything to file!
             * Storage may be full? */
            fclose(fd);
            unlink(filepath);

            ESP_LOGE(cModTag, "File write failed!");
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to write file to storage");
            return ESP_FAIL;
        }

        /* Keep track of remaining size of
         * the file left to be uploaded */
        remaining -= received;
    }

    /* Close file upon upload completion */
    fclose(fd);
    ESP_LOGI(cModTag, "File reception complete");

    /* Redirect onto root to see the updated file list */
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_sendstr(req, "File uploaded successfully");
    return ESP_OK;
}

/* Handler to delete a file from the server */
static esp_err_t delete_post_handler(httpd_req_t *req)
{
    char filepath[FILE_PATH_MAX];
    struct stat file_stat;

    /* Skip leading "/delete" from URI to get filename */
    /* Note sizeof() counts NULL termination hence the -1 */
    const char *filename = get_path_from_uri(filepath, ((struct file_server_data *)req->user_ctx)->base_path,
                                             req->uri  + sizeof("/deleteconfig") - 1, sizeof(filepath));
    if (!filename) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
        return ESP_FAIL;
    }

    /* Filename cannot have a trailing '/' */
    if (filename[strlen(filename) - 1] == '/') {
        ESP_LOGE(cModTag, "Invalid filename : %s", filename);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Invalid filename");
        return ESP_FAIL;
    }

    if (stat(filepath, &file_stat) == -1) {
        ESP_LOGE(cModTag, "File does not exist : %s", filename);
        /* Respond with 400 Bad Request */
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "File does not exist");
        return ESP_FAIL;
    }

    ESP_LOGI(cModTag, "Deleting file : %s", filename);
    /* Delete file */
    unlink(filepath);

    /* Redirect onto root to see the updated file list */
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_sendstr(req, "File deleted successfully");
    return ESP_OK;
}



const char * welcomePageRoute = "/welcome.html";

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

/* Send HTTP response with the contents of the requested file */
static esp_err_t pagePart_GetHandler(httpd_req_t *req) {
    char filepath[FILE_PATH_MAX];

    rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;
    strlcpy(filepath, rest_context->base_path, sizeof(filepath));

    if (req->uri[strlen(req->uri) - 1] == '/') { // Ends with '/'
        strlcat(filepath, "/welcome.html", sizeof(filepath));
    } else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }

    // Check if Rout misses .html-Ending -> Append .html
    if (strchr(filepath, '.') == NULL)
        strcpy(&filepath[strlen(filepath)], ".html"); 

    // Check if icon is requested -> Change to png
    char *iconPath = strstr(filepath, "/favicon.ico");
    if (iconPath != nullptr)
    {
        strcpy(iconPath, "/favicon.png");
    }

    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
        ESP_LOGE(cModTag, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char *chunk = rest_context->scratch;
    ssize_t read_bytes;
    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
        if (read_bytes == -1) {
            ESP_LOGE(cModTag, "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                close(fd);
                ESP_LOGE(cModTag, "File sending failed!");
                httpd_resp_sendstr_chunk(req, NULL);
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    /* Close file after sending complete */
    close(fd);
    ESP_LOGI(cModTag, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/**
 * @brief Get-Handlers for Get-Data-Requests
 *
 */
static const httpd_getUri_t getValueHandlers[] {
    {"/api/GetPort/RGBISync/", ProcessRgbiGet},
    {"/api/GetPort/RGBWAsync/", ProcessRgbwGet},
    {"/api/GetPort/RGBWSingle/", ProcessRgbwSingleGet},
    {"/api/GetPort/IValues/", ProcessGrayValuesGet},
    {"/api/GetStatus/WiFiStatus", ProcessWiFiStatusGet},
    {"/api/GetStatus/DeviceConfig", ProcessGetDeviceConfig},
    {"/api/GetStatus/DeviceType", ProcessGetDeviceType},
    {nullptr, nullptr},
} ;

static esp_err_t data_get_handler(httpd_req_t *req) {
    ESP_LOGI(cModTag, "Requested Uri: %s", req->uri);

    int idx = 0;
    size_t start = 0;
    const httpd_getUri_t *part = &getValueHandlers[idx++];
    while (part->uri != nullptr) {
        if (strstr(req->uri, part->uri) != nullptr) {
                start = strlen(part->uri);
                break;
            }
        part = &getValueHandlers[idx++];
    }

    if (part->uri == nullptr) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Source does not exist");
        return ESP_FAIL;
    }

    const int BufferSize = 256;
    int total_len = req->content_len;
    int cur_len = 0;
    char buf[BufferSize];
    int received = 0;
    if (total_len >= BufferSize) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }

    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            httpd_resp_send_err(
                req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    ESP_LOGI(cModTag, "=========== RECEIVED DATA ==========");
    ESP_LOGI(cModTag, "Raw: %s", &(req->uri[start]));
    ESP_LOGI(cModTag, "====================================");

    const char * output = nullptr;
    pProcessGet pFunc = (pProcessGet)part->pFunc;
    if ((pFunc( &(req->uri[start]) , &output) == ESP_OK) && (output != nullptr)) {
        httpd_resp_set_type(req, "application/json");
        httpd_resp_sendstr(req, output);

        ESP_LOGI(cModTag, "=========== Sending DATA ==========");
        ESP_LOGI(cModTag, "Raw: %s", output);
        ESP_LOGI(cModTag, "====================================");

    } else {
        httpd_resp_send_err(
            req, HTTPD_500_INTERNAL_SERVER_ERROR, "Something went wrong during processing");
    }
    
    if (output == nullptr) { 
        delete[] output; }
    return ESP_OK;
}

static const httpd_uri_t getPortReq = {
    .uri = "/api/*", .method = HTTP_GET, .handler = data_get_handler, .user_ctx = NULL};


/**
 * @brief Set-Handler for Set-Requests from Web-Gui
 *
 */
static const httpd_postUri_def postUriHandlers[]{
    {"/api/SetPort/RGBISync", ProcessRgbiPost},
    {"/api/SetPort/RGBWAsync", ProcessRgbwPost},
    {"/api/SetPort/RGBWSingle", ProcessRgbwSinglePost},
    {"/api/SetPort/IValues", ProcessGrayValuesPost},
    {"/api/LoadPage", ProcessLoadPage},
    {"/api/SaveToPage", ProcessSaveToPage},
    {"/api/ResetSavedPages", ProcessResetPages},
    {"/api/SetDevice/WiFiConnect",ProcessWiFiStatusSet},
    {"/api/SetDevice/ResetWiFiConnect", ProcessResetWifiConfig},
    {"/api/SetDevice/ResetDevice", ProcessRestartStation},
    {nullptr, nullptr},
};

/* An HTTP POST handler */
static esp_err_t data_post_handler(httpd_req_t *req) {
    ESP_LOGI(cModTag, "Posted Uri: %s", req->uri);

    int idx = 0;
    const httpd_postUri_def *part = &postUriHandlers[idx++];
    while (part->uri != nullptr) {
        if (strcmp(req->uri, part->uri) == 0)
            break;
        part = &postUriHandlers[idx++];
    }

    if (part->uri == nullptr) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Source does not exist");
        return ESP_FAIL;
    }

    pProcessPost pFunc = (pProcessPost)part->pFunc;

    const int BufferSize = 256;
    int total_len = req->content_len;
    int cur_len = 0;
    char buf[BufferSize];
    int received = 0;
    if (total_len >= BufferSize) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }

    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            httpd_resp_send_err(
                req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    ESP_LOGI(cModTag, "=========== RECEIVED DATA ==========");
    ESP_LOGI(cModTag, "Raw: %s", buf);
    ESP_LOGI(cModTag, "====================================");

    /* Executing registered Handler-Function */
    const char * output = nullptr;
    if (pFunc(buf, &output) == ESP_OK) {
        httpd_resp_sendstr(req, "Post control value successfully");
        xSemaphoreGive(xNewLedWebCommand);
    } else {
        httpd_resp_send_err(
            req, HTTPD_500_INTERNAL_SERVER_ERROR, "Something went wrong during Json parsing");
    }
    httpd_resp_send_chunk(req, NULL, 0);
        
    if (output == nullptr) { 
        delete[] output; }
    return ESP_OK;
}

static const httpd_uri_t setPortReq = {
    .uri = "/api/*", .method = HTTP_POST, .handler = data_post_handler, .user_ctx = NULL};


static httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    // Start the httpd server
    ESP_LOGI(cModTag, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(cModTag, "Registering URI handlers");
        httpd_register_uri_handler(server, &setPortReq);
        httpd_register_uri_handler(server, &getPortReq);
        return server;
    }

    ESP_LOGI(cModTag, "Error starting server!");
    return NULL;
}

void SetupMyWeb(QueueHandle_t setQ, SemaphoreHandle_t newWebCmd, pChannelGetCb getCbk, deviceConfig_t * stationConfig) {
    xNewLedWebCommand = newWebCmd;
    ESP_LOGI(cModTag, "Initialization of web interface ... ");

    rest_server_context_t *rest_context =
        (rest_server_context_t *)calloc(1, sizeof(rest_server_context_t));
    strncpy(rest_context->base_path, webBasePath, sizeof(rest_context->base_path));

    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());

    SetQueueHandlesForPostH(setQ, getCbk, stationConfig);

    /* Start the server for the first time */
    server = start_webserver();

    static struct file_server_data *server_data = NULL;
    /* Allocate memory for server data */
    server_data = (file_server_data *)calloc(1, sizeof(struct file_server_data));
    if (!server_data) {
        ESP_LOGE(cModTag, "Failed to allocate memory for server data");
        return; // ESP_ERR_NO_MEM;
    }
    strncpy(server_data->base_path, setupBasePath,
            sizeof(server_data->base_path));

    /* URI handler for uploading files to server */
    httpd_uri_t file_upload = {
        .uri       = "/uploadconfig/*",   // Match all URIs of type /upload/path/to/file
        .method    = HTTP_POST,
        .handler   = upload_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &file_upload);

    /* URI handler for deleting files from server */
    httpd_uri_t file_delete = {
        .uri       = "/deleteconfig/*",   // Match all URIs of type /delete/path/to/file
        .method    = HTTP_POST,
        .handler   = delete_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &file_delete);

    /* URI handler for getting uploaded files */
    httpd_uri_t file_download = { 
        .uri       = "/config/*",  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = download_get_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &file_download);

    /* URI handler for HTTP-Server */
    static httpd_uri_t pagePart = {
        .uri = "/*", .method = HTTP_GET, .handler = pagePart_GetHandler, .user_ctx = rest_context};
    httpd_register_uri_handler(server, &pagePart);

    return;
}


const char * GetConfigRoute = "/config/*";
const char * PostConfigRoute = "/uploadconfig/*";