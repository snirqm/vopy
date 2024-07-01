#
# Dependencies
#

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads)
include_directories(${Boost_INCLUDE_DIRS})
#
# Sources
#
set(SOURCES
        source/client.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})


# simulation-api-client
add_library(simulation-api-client SHARED ${SOURCES})
set_target_properties(simulation-api-client PROPERTIES
        COMPILE_OPTIONS "-Wno-pointer-arith"
)
#
target_include_directories(simulation-api-client
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${Boost_INCLUDE_DIRS}
)

#
# Libraries
#

message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
target_link_libraries(simulation-api-client
        PUBLIC
        commands
        ${Boost_LIBRARIES}
        Threads::Threads
)


# Install the headers
install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "client.h"
)

# simulation-api-client-app
#
# Target
#
set( CLI_SOURCES
        source/main.cpp
        source/app.cpp
)
add_executable(simulation-api-client-app ${CLI_SOURCES})

#
# Include directories
#
target_include_directories(simulation-api-client-app
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

#
# Libraries
#


target_link_libraries(simulation-api-client-app
        INTERFACE
        external
        PRIVATE
        simulation-api-client
        cli++
        easyloggingpp
        
        #        vopy-proto
)
