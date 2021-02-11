idf_component_register(SRCS "connect.c" "stdin_out.c" "addr_from_stdin.c"
                            "SoftAp.cpp"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES FileSystem esp_netif nvs_flash Utils
                    )
