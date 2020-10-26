add_library(dacTests OBJECT "dacTest.cpp" "dacMock.cpp" ${DUT_PARENT_DIR}/DacPort.cpp)
target_include_directories(dacTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(dacTests testUtils espMock)