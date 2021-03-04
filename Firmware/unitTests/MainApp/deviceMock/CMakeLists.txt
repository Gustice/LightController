project (deviceMock)

# Header only library, therefore INTERFACE
add_library(deviceMock INTERFACE deviceMock.cpp)

# INTERFACE targets only have INTERFACE properties
target_include_directories(deviceMock INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

