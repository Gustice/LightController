add_library(rmtTests OBJECT "rmtTest.cpp" "rmtMock.cpp" ${DUT_PARENT_DIR}/RmtPort.cpp)
target_include_directories(rmtTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(rmtTests testUtils espMock)