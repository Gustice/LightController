add_library(spiTests OBJECT "spiTest.cpp" "spiMock.cpp" ${DUT_PARENT_DIR}/SpiPort.cpp)
target_include_directories(spiTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(spiTests testUtils espMock)