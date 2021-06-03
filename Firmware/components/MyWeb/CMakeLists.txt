idf_component_register(SRCS  "MyWeb.cpp"
                       "ProcessHandler.cpp" "SourceReader.cpp" "GetFileList.c" "MappingParser.cpp"
                       INCLUDE_DIRS "includes" "private"
                       REQUIRES spiffs vfs nvs_flash newlib fatfs lwip esp_http_server MyConnect LightEffect json FileSystem 
                    #    EMBED_FILES "WebParts/index.html"
                       )