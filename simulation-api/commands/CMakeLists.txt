set(CMAKE_CXX_STANDARD 14)
enable_language(C)
set( SOURCES
        vopy_commands.c
)
add_library(commands SHARED ${SOURCES})
target_include_directories(commands
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

install(FILES vopy_commands.h DESTINATION include)