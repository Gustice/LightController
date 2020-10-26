add_library(gpioTests OBJECT "gpioTest.cpp" "gpioMock.cpp" ${DUT_PARENT_DIR}/GpioPort.cpp)
target_include_directories(gpioTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(gpioTests testUtils espMock)