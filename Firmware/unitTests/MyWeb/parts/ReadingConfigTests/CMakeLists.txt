add_library(ReadingConfigTests OBJECT "parseFileTest.cpp")
target_include_directories(ReadingConfigTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(ReadingConfigTests 
    testUtils 
    espMock 
    devMock 
    ${CMAKE_SOURCE_DIR}/libcjson.dll.a)