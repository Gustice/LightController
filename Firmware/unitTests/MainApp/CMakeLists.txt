cmake_minimum_required(VERSION 3.0)
project(MainAppTests)

add_subdirectory(include)
add_subdirectory(espMock)
add_subdirectory(deviceMock)

project(dut)
get_filename_component(DUT_PARENT_DIR ../../main ABSOLUTE)
get_filename_component(WEB_PARENT_DIR ../../components/MyWeb ABSOLUTE)
get_filename_component(READER_PARENT_DIR ../../components/FileSystem ABSOLUTE)
get_filename_component(CONNECTOR_PARENT_DIR ../../components/MyConnect ABSOLUTE)
get_filename_component(PARENT_INCLUDEDIR ../../components/LightEffect ABSOLUTE)

add_executable(TestRunner   "TestMain.cpp"
                            ${DUT_PARENT_DIR}"/Utils/DeviceDriver.cpp"
                            "parts/RotatingIndexTests.cpp"
                            "parts/ChannelIndexesTests.cpp"
                            )

target_include_directories(TestRunner PUBLIC testUtils 
                            ${CMAKE_CURRENT_LIST_DIR} 
                            ${DUT_PARENT_DIR}/Utils 
                            ${WEB_PARENT_DIR}/includes
                            ${DUT_PARENT_DIR}/private
                            ${PARENT_INCLUDEDIR}/includes
                            ${READER_PARENT_DIR}/include
                            ${CONNECTOR_PARENT_DIR}/include 
                            )

target_link_libraries(TestRunner 
    testUtils 
    espMock
    deviceMock
    )
