#
# Dependencies
#
find_package(Boost REQUIRED COMPONENTS system)
#
# Sources
#
set(SOURCES
        source/main.cpp
        source/app.cpp
        source/client.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

#
# Target
#
add_executable(simulation-api-client ${SOURCES})

#
# Include directories
#
target_include_directories(simulation-api-client
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external
        ${CMAKE_SOURCE_DIR}/commands

)

#
# Libraries
#


target_link_libraries(simulation-api-client
        INTERFACE
        commands
        external
        PRIVATE
        vopy-proto

)
