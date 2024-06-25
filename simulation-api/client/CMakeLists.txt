add_subdirectory(cpp)

add_custom_target(py-simulation-api-client
    COMMAND poetry build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python
    COMMENT "Building python package"
)