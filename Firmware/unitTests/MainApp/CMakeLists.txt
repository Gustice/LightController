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
get_filename_component(EFFECT_INCLUDEDIR ../../components/LightEffect ABSOLUTE)

# Header only library, therefore INTERFACE
add_library(effectProc STATIC   "${EFFECT_INCLUDEDIR}/Color.cpp" 
                                "${EFFECT_INCLUDEDIR}/Macro.cpp" 
                                "${EFFECT_INCLUDEDIR}/Processor.cpp" 
                                "${EFFECT_INCLUDEDIR}/Sequencer.cpp" 
                                "${EFFECT_INCLUDEDIR}/SequenceStepper.cpp" 
                                "${EFFECT_INCLUDEDIR}/StateMachine.cpp" 
                                "${EFFECT_INCLUDEDIR}/Waveforms.cpp" 
)
target_include_directories(effectProc PUBLIC "${EFFECT_INCLUDEDIR}/includes")

add_executable(TestRunner   "TestMain.cpp"
                            ${DUT_PARENT_DIR}"/Utils/DeviceDriver.cpp"
                            "parts/RotatingIndexTests.cpp"
                            "parts/ChannelIndexesTests.cpp"
                            "parts/DeviceDriverTests.cpp"
                            )

target_include_directories(TestRunner PUBLIC testUtils 
                            ${CMAKE_CURRENT_LIST_DIR} 
                            ${DUT_PARENT_DIR}/Utils 
                            ${WEB_PARENT_DIR}/includes
                            ${DUT_PARENT_DIR}/private
                            ${EFFECT_INCLUDEDIR}/includes
                            ${READER_PARENT_DIR}/include
                            ${CONNECTOR_PARENT_DIR}/include 
                            )

target_link_libraries(TestRunner 
    testUtils 
    espMock
    deviceMock
    effectProc
    )
