add_library(pwmTests OBJECT "pwmTest.cpp" "pwmMock.cpp" ${DUT_PARENT_DIR}/PwmPort.cpp)
target_include_directories(pwmTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(pwmTests testUtils espMock)