
add_library(cli++ INTERFACE cli.h)
target_include_directories(cli++
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_library(easyloggingpp SHARED easylogging++.cpp)
target_include_directories(easyloggingpp
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_compile_definitions(easyloggingpp PUBLIC AUTO_INITIALIZE_EASYLOGGINGPP)
