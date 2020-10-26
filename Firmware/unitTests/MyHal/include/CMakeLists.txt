project (testUtils)

# Header only library, therefore INTERFACE
add_library(testUtils INTERFACE)

# INTERFACE targets only have INTERFACE properties
target_include_directories(testUtils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(testUtils INTERFACE cxx_std_11)
