cmake_minimum_required(VERSION 3.0)
project(HyDeviceTests)

add_compile_options("/std:c++latest")

# DUT 
project (dut)
get_filename_component(DUT_PARENT_DIR ../../components/Devices ABSOLUTE)
get_filename_component(DUT_PARENT_DIR ../../components/Devices ABSOLUTE)

# The test Runner
include_directories(include)

add_library(MyLibrary_Static STATIC 
                            $<TARGET_OBJECTS:encoderTests> 
                            $<TARGET_OBJECTS:apa102Tests> 
                            $<TARGET_OBJECTS:pca9685Tests> 
                            $<TARGET_OBJECTS:ws2812Tests> 
                            $<TARGET_OBJECTS:rgbwStripTests> 
                            )
add_executable(TestRunner   $<TARGET_OBJECTS:encoderTests> 
                            $<TARGET_OBJECTS:apa102Tests> 
                            $<TARGET_OBJECTS:pca9685Tests> 
                            $<TARGET_OBJECTS:ws2812Tests> 
                            $<TARGET_OBJECTS:rgbwStripTests> 
                            TestMain.cpp 
                            )
target_include_directories(TestRunner PUBLIC testUtils devMock) 
add_test(NAME DeviceTests COMMAND 
                encoderTests 
                apa102Tests 
                pca9685Tests 
                ws2812Tests 
                rgbwStripTests 
                )


add_subdirectory(include)
add_subdirectory(parts)
# add_subdirectory(espMock)
add_subdirectory(hwMock)
