
#include "includes/MyWeb.h"

#include "cJSON.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "protocol_common.h"
#include <esp_event.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <sstream>
#include <sys/param.h>
#include <sys/stat.h>
#include "ProcessHandler.h"

#include "ColorPosts.h"
#include "SoftAp.h"
#include "WebUtils.h"


static const char *cModTag = "WEB";

/**
 * @brief Get-Handler für Page-Content
 * 
 */
static const httpd_uriPart_t getUriHandlers[]{
    {"/", (void *)FileSource_index_html},
    {"/RgbSetupFull", (void *)FileSource_RgbSetupFull_html},
    {"/RgbSetupFull.html", (void *)FileSource_RgbSetupFull_html},
    {"/WiFiConnectInput", (void *)FileSource_WiFiConnectInput_html},
    {"/WiFiConnectInput.html", (void *)FileSource_WiFiConnectInput_html},
    {"/favicon.ico", (void *)FileSource_favicon_ico}, 
    {"/CommonScripts.js", (void *)FileSource_CommonScripts_js},
    {"/CommonStyles.css", (void *)FileSource_CommonStyles_css},
    {"/RgbiControl.html", (void *)FileSource_RgbiControl_html},
    {"/RgbwControl.html", (void *)FileSource_RgbwControl_html},
    {"/RgbSingleControl.html", (void *)FileSource_RgbSingleControl_html},
    {"/GrayControl.html", (void *)FileSource_GrayControl_html},
    {nullptr, nullptr}, // Terminator
};

RgbPost *rgbPostCh1;
Color_t color;
uint8_t intensity;
SemaphoreHandle_t xNewLedWebCommand;

/* Handler to download a file kept on the server */
static esp_err_t pagePart_GetHandler(httpd_req_t *req) {
    ESP_LOGI(cModTag, "Requested Uri: %s", req->uri);

    int idx = 0;
    const httpd_uriPart_t *part = &getUriHandlers[idx++];
    while (part->uri != nullptr) {
        if (strcmp(req->uri, part->uri) == 0)
            break;
        part = &getUriHandlers[idx++];
    }

    if (part->uri == nullptr) {
        /* Respond with 404 Not Found */
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Page-Source does not exist");
        return ESP_FAIL;
    }

    const FileContext_t pageSource = ((fSourceHandle)part->user_ctx)();

    httpd_resp_set_type(req, pageSource.type);
    httpd_resp_send(req, (const char *)pageSource.hFile, pageSource.size);

    return ESP_OK;
}

static httpd_uri_t pagePart = {
    .uri = "/*", .method = HTTP_GET, .handler = pagePart_GetHandler, .user_ctx = NULL};


/**
 * @brief Set-Handler for Set-Requests from Web-Gui
 * 
 */
static const httpd_postUri_def postUriHandlers[]{
    {"/SetPort/RGBIValues", ProcessRgbiPost},
    {"/SetPort/RGBWValues", ProcessRgbwPost},
    {"/SetPort/RGBWSingleValue", ProcessRgbwSinglePost},
    {"/SetPort/IValues", ProcessGrayValuesPost},
    {"/SaveToPage", ProcessSaveToPage},
    {"/ResetProgramm", ProcessResetPages},
    {nullptr, nullptr},
};

esp_err_t ProcessSaveToPage(const char *message) {
    int page = std::stoi(message);
    return ESP_OK;
}

esp_err_t ProcessResetPages(const char *messange) {
    return ESP_OK;
}

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
        /* Respond with 404 Not Found */
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
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
                                "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    ESP_LOGI(cModTag, "=========== RECEIVED DATA ==========");
    ESP_LOGI(cModTag, "Raw: %s", buf);
    ESP_LOGI(cModTag, "====================================");

    /* Executing registered Handler-Function */
    if (pFunc(buf) == ESP_OK)
    {
        httpd_resp_sendstr(req, "Post control value successfully");
        xSemaphoreGive( xNewLedWebCommand );
    }
    else
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Something went wrong during Json parsing");
    }

    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t setPortReq = {
    .uri = "/SetPort/*", .method = HTTP_POST, .handler = data_post_handler, .user_ctx = NULL};


/**
 * @brief Get-Handlers for Get-Data-Requests
 * 
 */
static const httpd_getUri_t getValueHandlers[]{
    {"/GetValues/RGBIValues", ProcessRgbiGet},
    {"/GetValues/RGBWValues", ProcessRgbwGet},
    {"/GetValues/RGBWSingleValue", ProcessRgbwSingleGet},
    {"/GetValues/IValues", ProcessGrayValuesGet},
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
        /* Respond with 404 Not Found */
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Source does not exist");
        return ESP_FAIL;
    }

    // todo Implement Function-Call here
    const char *output = "This is a Test";
    httpd_resp_send(req, output, strlen(output));
    return ESP_OK;
}

static const httpd_uri_t getPortReq = {
    .uri = "/GetValues/*", .method = HTTP_GET, .handler = data_get_handler, .user_ctx = NULL};



/* An HTTP POST handler */
static esp_err_t config_post_handler(httpd_req_t *req) {
    ESP_LOGI(cModTag, "Posted Uri: %s", req->uri);

    const int BufferSize = 256;
    int total_len = req->content_len;
    int cur_len = 0;
    char buf[BufferSize];
    int received = 0;
    if (total_len >= BufferSize) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }

    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
                                "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    ESP_LOGI(cModTag, "=========== RECEIVED DATA ==========");
    ESP_LOGI(cModTag, "Raw: %s", buf);
    ESP_LOGI(cModTag, "====================================");

    wifiApConfig_t config;
    memset(&config, 0, sizeof(wifiApConfig_t));

    cJSON *root = cJSON_Parse(buf);
    const char *ssid = cJSON_GetObjectItem(root, "ssid")->valuestring;
    const char *password = cJSON_GetObjectItem(root, "password")->valuestring;
    strcpy((char *)config.ssid, ssid);
    strcpy((char *)config.password, password);
    cJSON_Delete(root);

    if (SaveWiFiConfig(&config) == ESP_OK) {
        httpd_resp_sendstr(req, "Post control value successfully");
        httpd_resp_send_chunk(req, NULL, 0);
        return ESP_OK;

    } else {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Post control saving failed");
        httpd_resp_send_chunk(req, NULL, 0);
        return ESP_FAIL;
    }
}

static const httpd_uri_t setDeviceReq = {.uri = "/SetDevice/WiFiConnect",
                                         .method = HTTP_POST,
                                         .handler = config_post_handler,
                                         .user_ctx = NULL};

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
        httpd_register_uri_handler(server, &setDeviceReq);
        httpd_register_uri_handler(server, &pagePart); // Most generic Page
        return server;
    }

    ESP_LOGI(cModTag, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server) {
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                               void *event_data) {
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server) {
        ESP_LOGI(cModTag, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                            void *event_data) {
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server == NULL) {
        ESP_LOGI(cModTag, "Starting webserver");
        *server = start_webserver();
    }
}

void SetupMyWeb(QueueHandle_t colorQ, QueueHandle_t grayQ, SemaphoreHandle_t newWebCmd ) {
    xNewLedWebCommand = newWebCmd;
    ESP_LOGI(cModTag, "Initialization of web interface ... ");
    static httpd_handle_t server = NULL;
    rgbPostCh1 = new RgbPost(3); // todo change

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());

    SetQueueHandlesForPostH(colorQ, grayQ);

    /* Start the server for the first time */
    server = start_webserver();
}
