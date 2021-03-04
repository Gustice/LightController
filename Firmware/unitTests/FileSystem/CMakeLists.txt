cmake_minimum_required(VERSION 3.0)
project(MyWebTests)

add_subdirectory(cJSON)
add_subdirectory(include)
add_subdirectory(espMock)

project (dut)
get_filename_component(DUT_PARENT_DIR ../../components/FileSystem ABSOLUTE)
get_filename_component(PARENT_INCLUDEDIR ../../components/LightEffect ABSOLUTE)

add_compile_definitions(CONFIG_FACTORY_IMAGE_FILENAME="Device.json")
add_compile_definitions(CONFIG_DEVICE_CONFIGURATION_FILENAME="TestConfig.json")
add_compile_definitions(CONFIG_FS_ROOT="myFs")

message( "### ALl Sources " ${sources})
add_executable(TestRunner   "TestMain.cpp"
                            "ReaderMock.cpp"
                            ${DUT_PARENT_DIR}"/ParamReader.cpp"
                            "parts/ParseFileTest.cpp"
                            )
target_include_directories(TestRunner PUBLIC testUtils cJSON
                            ${CMAKE_CURRENT_LIST_DIR} 
                            ${DUT_PARENT_DIR}/include 
                            ${DUT_PARENT_DIR}/private
                            ${PARENT_INCLUDEDIR}/includes
                            )
    
target_link_libraries(TestRunner 
    testUtils 
    json
    espMock
    )
include_directories(cJSON)