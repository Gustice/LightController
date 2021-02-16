idf_component_register(SRCS  "MyWeb.cpp"
                        "ColorPosts.cpp" "ProcessHandler.cpp" "SourceReader.cpp"
                       INCLUDE_DIRS "includes" "private"
                       REQUIRES spiffs nvs_flash fatfs lwip esp_http_server MyConnect LightEffect json FileSystem
                    #    EMBED_FILES "WebParts/index.html"
                       )