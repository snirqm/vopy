cmake_minimum_required(VERSION 3.15...3.26)

project(simulation-api LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(commands)
add_subdirectory(proto)
add_subdirectory(client)
add_subdirectory(server)
