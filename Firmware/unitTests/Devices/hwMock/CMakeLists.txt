project (devMock)
add_library(devMock 
            common.cpp 
            )
target_include_directories(devMock INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(devMock PUBLIC catch)

