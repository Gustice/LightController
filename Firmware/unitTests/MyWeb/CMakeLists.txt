cmake_minimum_required(VERSION 3.0)
project(HyHalTests)

# The test Runner
add_executable(TestRunner TestMain.cpp 
                            parts/colorPostsTest.cpp 
                            )
target_link_libraries(TestRunner PUBLIC catch dut 
# color
)

add_subdirectory(include)

get_filename_component(PARENT_DIR ../../components/MyWeb ABSOLUTE)
get_filename_component(PARENT_INCLUDEDIR ../../components/LightEffect ABSOLUTE)
project (dut)

add_library(dut ${PARENT_DIR}/ColorPosts.cpp)
target_include_directories(dut PUBLIC ${PARENT_DIR}/includes ${PARENT_INCLUDEDIR}/includes )

# project (color)
# add_library(color ${PARENT_INCLUDEDIR}/Color.cpp)
# target_include_directories(color PUBLIC $ ${PARENT_INCLUDEDIR}/includes )