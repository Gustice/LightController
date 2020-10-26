add_library(adcTests OBJECT "adcTest.cpp" "adcMock.cpp" ${DUT_PARENT_DIR}/AdcPort.cpp)
target_include_directories(adcTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(adcTests testUtils espMock)