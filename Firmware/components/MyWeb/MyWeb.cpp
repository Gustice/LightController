
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
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <fcntl.h>
#include <nvs_flash.h>
#include <sstream>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>

#include "ColorPosts.h"
#include "ParamReader.h"
#include "SoftAp.h"
#include "WebUtils.h"


static const char *cModTag = "WEB";
static const char *webBasePath = CONFIG_WEB_MOUNT_POINT;
#define FILE_PATH_MAX   (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)

RgbPost *rgbPostCh1;
Color_t color;
uint8_t intensity;
SemaphoreHandle_t xNewLedWebCommand;

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

/* Send HTTP response with the contents of the requested file */
static esp_err_t pagePart_GetHandler(httpd_req_t *req) {
    char filepath[FILE_PATH_MAX];

    rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;
    strlcpy(filepath, rest_context->base_path, sizeof(filepath));

    // Check if Rout misses .html-Ending -> Append .html
    if (strchr(filepath, '.') == NULL)
        strcpy(&filepath[strlen(filepath)], ".html"); 

    if (req->uri[strlen(req->uri) - 1] == '/') {
        strlcat(filepath, "/welcome.html", sizeof(filepath));
    } else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }
    // todo expand path without .html to html

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
static const httpd_getUri_t getValueHandlers[]{
    {"/api/GetPort/RGBISync", ProcessRgbiGet},
    {"/api/GetPort/RGBWAsync", ProcessRgbwGet},
    {"/api/GetPort/RGBWSingle", ProcessRgbwSingleGet},
    {"/api/GetPort/IValues", ProcessGrayValuesGet},
    {"/api/GetStatus/WiFiStatus", ProcessWiFiStatusGet},
    {"/api/GetStatus/DeviceConfig", ProcessGetDeviceConfig},
    {nullptr, nullptr},
};

static esp_err_t data_get_handler(httpd_req_t *req) {
    ESP_LOGI(cModTag, "Requested Uri: %s", req->uri);

    int idx = 0;
    const httpd_getUri_t *part = &getValueHandlers[idx++];
    while (part->uri != nullptr) {
        if (strcmp(req->uri, part->uri) == 0)
            break;
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
    ESP_LOGI(cModTag, "Raw: %s", buf);
    ESP_LOGI(cModTag, "====================================");

    char * output = nullptr;
    pProcessGet pFunc = (pProcessGet)part->pFunc;
    if ((pFunc(buf, &output) == ESP_OK) && (output != nullptr)) {
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
    {"/api/SaveToPage", ProcessSaveToPage},
    {"/api/ResetProgram", ProcessResetPages},
    {"/api/SetDevice/WiFiConnect",ProcessWiFiStatusSet},
    {"/api/SetDevice/ResetWiFiConnect", ResetWifiConfig},
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
    if (pFunc(buf) == ESP_OK) {
        httpd_resp_sendstr(req, "Post control value successfully");

        xSemaphoreGive(xNewLedWebCommand);
    } else {
        httpd_resp_send_err(
            req, HTTPD_500_INTERNAL_SERVER_ERROR, "Something went wrong during Json parsing");
    }
    httpd_resp_send_chunk(req, NULL, 0);
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
        // httpd_register_uri_handler(server, &pagePart); // Most generic Page
        return server;
    }

    ESP_LOGI(cModTag, "Error starting server!");
    return NULL;
}

void SetupMyWeb(QueueHandle_t colorQ, QueueHandle_t grayQ, SemaphoreHandle_t newWebCmd) {
    xNewLedWebCommand = newWebCmd;
    ESP_LOGI(cModTag, "Initialization of web interface ... ");

    rest_server_context_t *rest_context =
        (rest_server_context_t *)calloc(1, sizeof(rest_server_context_t));
    strlcpy(rest_context->base_path, webBasePath, sizeof(rest_context->base_path));

    static httpd_handle_t server = NULL;
    rgbPostCh1 = new RgbPost(3); // todo change

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());

    SetQueueHandlesForPostH(colorQ, grayQ);

    /* Start the server for the first time */
    server = start_webserver();

    static httpd_uri_t pagePart = {
        .uri = "/*", .method = HTTP_GET, .handler = pagePart_GetHandler, .user_ctx = rest_context};
    httpd_register_uri_handler(server, &pagePart);

    return;
}
