add_library(ws2812Tests OBJECT "ws2812Test.cpp" ${DUT_PARENT_DIR}/Ws2812.cpp)
target_include_directories(ws2812Tests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(ws2812Tests testUtils espMock devMock)