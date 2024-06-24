project(commands C)

set(CMAKE_CXX_STANDARD 14)
# this project contains only single header file
add_library(commands INTERFACE)
target_include_directories(commands INTERFACE include)
