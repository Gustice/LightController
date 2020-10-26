#pragma once

#include "esp_err.h"

typedef struct FileContext_def {
    const char *key;
    const unsigned char *hFile;
    const size_t size;
    const char *type;
} FileContext_t;

typedef const FileContext_t (*fSourceHandle)(void);
typedef esp_err_t (*pProcessPost)(const char *);
typedef esp_err_t (*pProcessGet)(char *);

typedef struct httpd_uriPart_def {
    /** The URI to handle */
    const char *uri;
    /** Pointer to user context data which will be available to handler */
    void *user_ctx;
} httpd_uriPart_t;
typedef struct httpd_postUri_def {
    const char *uri;
    pProcessPost pFunc;
} httpd_postUri_t;
typedef struct httpd_getUri_def {
    const char *uri;
    pProcessGet pFunc;
} httpd_getUri_t;


/**
 * @brief Call-Construct to integrated included text ressources by assembler.
 * @example: FILESource(main_html, html)
 * @note type is text type not file extension
 */
#define FILESource(fileName, fileType, content)                                                    \
    const FileContext_t FileSource_##fileName(void) {                                              \
        extern const unsigned char fileStart_##fileName[] asm("_binary_" #fileName "_start");      \
        extern const unsigned char fileEnd_##fileName[] asm("_binary_" #fileName "_end");          \
        const static FileContext_t scriptsSource{                                                  \
            .key = content,                                                                        \
            .hFile = fileStart_##fileName,                                                         \
            .size = (size_t)(fileEnd_##fileName - fileStart_##fileName),                           \
            .type = fileType,                                                                      \
        };                                                                                         \
        return scriptsSource;                                                                      \
    }

FILESource(RgbSetupFull_html, "text/html", "page") // expands to: FileSource_RgbSetupFull_html_html
//< const FileContext_t FileSource_MyPage_html(void) {
//<    extern const unsigned char MyWeb_start[] asm("_binary_MyPage_html_start");
//<    extern const unsigned char MyWeb_end[]   asm("_binary_MyPage_html_end");
//<    const static FileContext_t scriptsSource {
//<       .key = "scripts",
//<       .hFile = MyWeb_start,
//<       .size = (size_t)(MyWeb_end - MyWeb_start),
//<       .type = "text/html",
//<     };
//<     return scriptsSource;
//< }

FILESource(index_html, "text/html", "page")
FILESource(CommonScripts_js, "text/html", "scripts")
FILESource(favicon_ico, "image/x-icon", "styles")
FILESource(CommonStyles_css, "text/css", "styles")
FILESource(RgbiControl_html, "text/html", "control")
FILESource(RgbwControl_html, "text/html", "control")
FILESource(RgbSingleControl_html, "text/html", "control")
FILESource(GrayControl_html, "text/html", "control")
FILESource(WiFiConnectInput_html, "text/html", "page")