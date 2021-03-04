project (espMock)

# Header only library, therefore INTERFACE
add_library(espMock INTERFACE common.cpp)

# INTERFACE targets only have INTERFACE properties
target_include_directories(espMock INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

