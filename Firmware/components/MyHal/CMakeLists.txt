idf_component_register(SRCS "UartPort.cpp" 
                            "RmtPort.cpp" 
                            "GpioPort.cpp" 
                            "AdcPort.cpp" 
                            "DacPort.cpp" 
                            "I2cPort.cpp" 
                            "PwmPort.cpp" 
                            "SpiPort.cpp" 
                            "RmtPort.cpp" 
                            "ir_builder_rmt_nec.c" "ir_parser_rmt_nec.c"
                            "ir_parser_rmt_rc5.c" "ir_builder_rmt_rc5.c"
                       INCLUDE_DIRS "includes" "private"
                       REQUIRES esp_adc_cal
                       )
