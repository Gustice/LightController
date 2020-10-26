idf_component_register(SRCS 
            "Apa102.cpp"
            "Pca9685.cpp"
            "Ws2812.cpp"
            "RgbwStrip.cpp"
            "Encoder.cpp" 
            INCLUDE_DIRS "includes"
            REQUIRES LightEffect MyHal
            )
