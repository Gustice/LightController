cmake_minimum_required(VERSION 3.0)
project(MyWebTests)

add_subdirectory(cJSON)
add_subdirectory(include)
add_subdirectory(espMock)

project (dut)
get_filename_component(DUT_PARENT_DIR ../../components/MyWeb ABSOLUTE)
get_filename_component(READER_PARENT_DIR ../../components/FileSystem ABSOLUTE)
get_filename_component(CONNECTOR_PARENT_DIR ../../components/MyConnect ABSOLUTE)
get_filename_component(PARENT_INCLUDEDIR ../../components/LightEffect ABSOLUTE)

add_compile_definitions(CONFIG_DEVICE_CONFIGURATION_FILENAME="DeviceConfig.json")
add_compile_definitions(CONFIG_FACTORY_IMAGE_FILENAME="Device.json")

add_executable(TestRunner   "TestMain.cpp"
                            "ReaderMock.cpp"
                            ${DUT_PARENT_DIR}"/ProcessHandler.cpp"
                            ${DUT_PARENT_DIR}"/MappingParser.cpp"
                            "parts/ParseApplyToStringTests.cpp"
                            "parts/SetRequestTests.cpp"
                            "parts/GetRequestTests.cpp"
                            "parts/parseFileTest.cpp"
                            "parts/SaveLoadPageTest.cpp"
                            )
target_include_directories(TestRunner PUBLIC testUtils cJSON
                            ${CMAKE_CURRENT_LIST_DIR} 
                            ${DUT_PARENT_DIR}/includes 
                            ${DUT_PARENT_DIR}/private
                            ${PARENT_INCLUDEDIR}/includes
                            ${READER_PARENT_DIR}/include
                            ${CONNECTOR_PARENT_DIR}/include 
                            )
    
target_link_libraries(TestRunner 
    testUtils 
    json
    espMock
    )
include_directories(cJSON)