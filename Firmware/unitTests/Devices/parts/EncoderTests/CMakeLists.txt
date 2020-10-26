add_library(encoderTests OBJECT "encoderTest.cpp" ${DUT_PARENT_DIR}/Encoder.cpp)
target_include_directories(encoderTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(encoderTests testUtils espMock devMock)