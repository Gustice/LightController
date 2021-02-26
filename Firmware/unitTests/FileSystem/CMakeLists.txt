cmake_minimum_required(VERSION 3.0)
project(MyWebTests)

add_subdirectory(cJSON)
add_subdirectory(include)
add_subdirectory(espMock)

project (dut)
get_filename_component(DUT_PARENT_DIR ../../components/FileSystem ABSOLUTE)

message( "### ALl Sources " ${sources})
add_executable(TestRunner   "TestMain.cpp"
                            "ReaderMock.cpp"
                            ${DUT_PARENT_DIR}"/ParamReader.cpp"
                            "parts/ParseFileTest.cpp"
                            )
target_include_directories(TestRunner PUBLIC testUtils cJSON
                            ${CMAKE_CURRENT_LIST_DIR} 
                            ${DUT_PARENT_DIR}/includes 
                            ${DUT_PARENT_DIR}/private
                            )
    
target_link_libraries(TestRunner 
    testUtils 
    json
    espMock
    )
include_directories(cJSON)