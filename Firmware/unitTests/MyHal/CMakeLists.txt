cmake_minimum_required(VERSION 3.0)
project(HyHalTests)

# DUT 
project (dut)
get_filename_component(DUT_PARENT_DIR ../../components/MyHal ABSOLUTE)

# The test Runner
include_directories(include)

add_library(MyLibrary_Static STATIC 
                            $<TARGET_OBJECTS:gpioTests> 
                            $<TARGET_OBJECTS:adcTests> 
                            $<TARGET_OBJECTS:dacTests> 
                            $<TARGET_OBJECTS:i2cTests> 
                            $<TARGET_OBJECTS:pwmTests> 
                            $<TARGET_OBJECTS:spiTests> 
                            $<TARGET_OBJECTS:rmtTests> 
                            $<TARGET_OBJECTS:uartTests> 
                            )
add_executable(TestRunner   $<TARGET_OBJECTS:gpioTests> 
                            $<TARGET_OBJECTS:adcTests> 
                            $<TARGET_OBJECTS:dacTests> 
                            $<TARGET_OBJECTS:i2cTests> 
                            $<TARGET_OBJECTS:pwmTests> 
                            $<TARGET_OBJECTS:spiTests> 
                            $<TARGET_OBJECTS:rmtTests> 
                            $<TARGET_OBJECTS:uartTests> 
                            TestMain.cpp 
                            )
target_include_directories(TestRunner PUBLIC testUtils)
add_test(NAME HalTest COMMAND 
                gpioTests 
                adcTests 
                dacTests 
                i2cTests 
                pwmTests 
                spiTests 
                rmtTests 
                uartTests
                )


add_subdirectory(include)
add_subdirectory(parts)
add_subdirectory(espMock)