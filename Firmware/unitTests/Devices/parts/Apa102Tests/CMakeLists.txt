add_library(apa102Tests OBJECT "apa102Test.cpp" ${DUT_PARENT_DIR}/Apa102.cpp)
target_include_directories(apa102Tests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(apa102Tests testUtils espMock devMock)