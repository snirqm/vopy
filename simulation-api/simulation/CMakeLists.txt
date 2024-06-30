# Find VCS
enable_language(C)
find_program(VCS_EXECUTABLE NAMES vcs PATHS /tools/snps/vcs/current/bin)
if (NOT SVDPI_INCLUDE_DIR)
        message(FATAL_ERROR "SVDPI_INCLUDE_DIR not defined")
else()
        include_directories(${SVDPI_INCLUDE_DIR})
endif()

if(NOT VCS_EXECUTABLE)
        message(FATAL_ERROR "VCS not found")
endif()

set( SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/source/simulation.cpp
)

add_library(simulation-common SHARED ${SOURCES})
target_link_libraries(simulation-common simulation-api-server commands ${Boost_LIBRARIES})
target_include_directories(
        simulation-common
        PUBLIC
        ${CMAKE_SOURCE_DIR}/commands
        ${CMAKE_SOURCE_DIR}/server/include
        ${Boost_INCLUDE_DIR}
)

# check if FILE_LIST_FILE is defined
if(NOT DEFINED FILE_LIST_FILE)
        set(VERILOG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/system_verilog/dummy.sv)
else()
        set(VERILOG_FILES "-f ${FILE_LIST_FILE}")
endif()

if (NOT DEFINED SIMULATION_VERSION) 
        set(SIMULATION_VERSION "dev")
endif()

set (SIMULATION_DIR_NAME simulation-${SIMULATION_VERSION})
# Define the custom target for VCS compilation
set(SIMULATION_BUILD_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/build.csh)
set (VCS_ARGS 
        -full64 -sverilog -l mcomp.log -timescale=1ns/1ps -kdb 
        ${VERILOG_FILES}
        ${SOURCES}
        -CFLAGS -I${CMAKE_SOURCE_DIR}/commands
        -CFLAGS -I${CMAKE_SOURCE_DIR}/server/include
        -CFLAGS -I${Boost_INCLUDE_DIR}
        -LDFLAGS -L${CMAKE_BINARY_DIR}/simulation -lsimulation-common
        -LDFLAGS -L${CMAKE_BINARY_DIR}/server -lsimulation-api-server
        +plusarg_save
)



# Add custom target that depends on simulation-api-server target
add_custom_target(simulation ALL
        COMMAND ${SIMULATION_BUILD_SCRIPT} ${VCS_ARGS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${VERILOG_FILES} ${SOURCES}
        COMMENT "Compiling simulation with VCS"
)
add_dependencies(simulation simulation-api-server simulation-common)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/run.csh.in ${CMAKE_CURRENT_BINARY_DIR}/run.csh @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/run.sh.in ${CMAKE_CURRENT_BINARY_DIR}/run.sh @ONLY)

