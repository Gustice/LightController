add_library(rgbwStripTests OBJECT "rgbwStripTest.cpp" ${DUT_PARENT_DIR}/RgbwStrip.cpp)
target_include_directories(rgbwStripTests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(rgbwStripTests testUtils espMock devMock)