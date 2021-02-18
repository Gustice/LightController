message ("#######" ${DUT_PARENT_DIR})
add_library(ProcessHandlerTests OBJECT 
                "SetRequestTests.cpp" 
                ${DUT_PARENT_DIR}"/ProcessHandler.cpp")
target_include_directories( ProcessHandlerTests PRIVATE 
                ${CMAKE_CURRENT_LIST_DIR} 
                ${DUT_PARENT_DIR}/includes 
                ${DUT_PARENT_DIR}/private
                ${PARENT_INCLUDEDIR}/includes
                ${READER_PARENT_DIR}/include
                ${CONNECTOR_PARENT_DIR}/include )
target_link_libraries(ProcessHandlerTests 
    testUtils 
    espMock 
    devMock 
    ${CMAKE_SOURCE_DIR}/libcjson.dll.a
    )