#
# Dependencies
#
find_package(Threads)
find_package(Boost REQUIRED COMPONENTS system)
#
# Sources
#
set(SOURCES
        source/app.cpp
        source/main.cpp
        source/commands.cpp
        source/tcp-server.cpp
        source/file-server.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

#
# Target
#
add_executable(server ${SOURCES})

#
# Include directories
#
target_include_directories(server
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external
        ${CMAKE_SOURCE_DIR}/commands

)

#
# Libraries
#


target_link_libraries(server
        INTERFACE
        commands
        PRIVATE
        vopy

)
