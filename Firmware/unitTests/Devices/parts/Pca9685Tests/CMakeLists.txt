add_library(pca9685Tests OBJECT "pca9685Test.cpp" ${DUT_PARENT_DIR}/Pca9685.cpp)
target_include_directories(pca9685Tests PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${DUT_PARENT_DIR}/includes)
target_link_libraries(pca9685Tests testUtils espMock devMock)