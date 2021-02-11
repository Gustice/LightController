cmake_minimum_required(VERSION 3.0)
project(MyWebTests)


add_subdirectory(include)
add_subdirectory(parts)
# add_subdirectory(cJSON) # do not include !!! Included by DLL
                            # 1. Compiel cJSON 
                            # 2. Add output dll to build folder
# searching for library file
add_library(cJson SHARED cJSON/build/libcjson.dll)
include_directories(cJSON)
set_target_properties(cJson PROPERTIES LINKER_LANGUAGE C)

project (dut)
get_filename_component(DUT_PARENT_DIR ../../components/MyWeb ABSOLUTE)
get_filename_component(PARENT_INCLUDEDIR ../../components/LightEffect ABSOLUTE)

add_library(MyLibrary_Static STATIC 
                            $<TARGET_OBJECTS:ReadingConfigTests> 
                            )
add_executable(TestRunner   $<TARGET_OBJECTS:ReadingConfigTests> 
                            TestMain.cpp 
                            )

target_link_libraries(TestRunner testUtils cJson)
target_include_directories(TestRunner PUBLIC testUtils) 

add_test(NAME DeviceTests COMMAND 
                ReadingConfigTests 
                )