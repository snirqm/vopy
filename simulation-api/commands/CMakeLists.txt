# Create an interface library
add_library(commands INTERFACE)

# Set include directories for the interface library
target_include_directories(commands
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)