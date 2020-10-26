add_library(i2cTests OBJECT "i2cTest.cpp" "i2cMock.cpp" ${DUT_PARENT_DIR}/I2cPort.cpp)
target_include_directories(i2cTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(i2cTests testUtils espMock)