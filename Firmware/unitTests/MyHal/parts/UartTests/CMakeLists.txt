add_library(uartTests OBJECT "uartTest.cpp" "uartMock.cpp" ${DUT_PARENT_DIR}/UartPort.cpp)
target_include_directories(uartTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(uartTests testUtils espMock)