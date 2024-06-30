cmake_minimum_required(VERSION 3.15...3.26)

project(simulation-api LANGUAGES CXX C VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
find_package(Threads REQUIRED)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
add_subdirectory(commands)
# add_subdirectory(proto)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(simulation)
add_subdirectory(external)

# create a target to build server and client
add_custom_target(simulation-api DEPENDS
        simulation-api-server-app
        simulation-api-server
        simulation
        simulation-api-client
        simulation-api-client-app
)
include(client_package.cmake)
