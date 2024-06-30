#
# Dependencies
#
find_package(Threads)
find_package(Boost REQUIRED COMPONENTS system)


#
# Sources
#
set(SOURCES
        source/server.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

#
# Target
#
add_library(simulation-api-server SHARED ${SOURCES})

set_target_properties(simulation-api-server PROPERTIES
        COMPILE_OPTIONS "-Wno-pointer-arith"
)
#
# Libraries
#


target_link_libraries(simulation-api-server      
        PUBLIC        
        easyloggingpp
        PRIVATE
        ${Boost_LIBRARIES}
)

#
# Include directories
#
target_include_directories(simulation-api-server
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/commands
)

set( CLI_SOURCES
        source/app.cpp
        source/main.cpp
)

add_executable(simulation-api-server-app ${CLI_SOURCES})
target_link_libraries(simulation-api-server-app
        PRIVATE
        simulation-api-server
        cli++
        ${Boost_LIBRARIES}
        Threads::Threads
)

target_include_directories(simulation-api-server-app
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
