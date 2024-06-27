#
# Dependencies
#

find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})
#
# Sources
#
set(SOURCES
        source/tcp-client.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})


# simulation-api-client
add_library(simulation-api-client SHARED ${SOURCES})

target_include_directories(simulation-api-client
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

#
# Libraries
#


target_link_libraries(simulation-api-client
        PUBLIC
        commands
        PRIVATE
        ${Boost_LIBRARIES}
)


# Install the headers
install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "client.h"
)

# simulation-api-client-cli
#
# Target
#
set( CLI_SOURCES
        source/main.cpp
        source/app.cpp
)
add_executable(simulation-api-client-cli ${CLI_SOURCES})

#
# Include directories
#
target_include_directories(simulation-api-client-cli
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#
# Libraries
#


target_link_libraries(simulation-api-client-cli
        INTERFACE
        external
        PRIVATE
        commands
        simulation-api-client
        easyloggingpp
        ${Boost_LIBRARIES}
        #        vopy-proto
)
