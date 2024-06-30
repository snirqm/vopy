
set(CPACK_PACKAGE_NAME "simulation-api-client")
set(CPACK_PACKAGE_VENDOR "QM")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A client for the simulation API")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SimulationAPIClient")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

install(TARGETS simulation-api-client commands
        EXPORT simulation-api-client-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)




# Generate and install export file
install(EXPORT simulation-api-client-targets
        FILE simulation-api-client-targets.cmake
        NAMESPACE SimulationAPI::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simulation-api-client
)

# Generate the package configuration file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/simulation-api-client-config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simulation-api-client
)

# Generate the version file for the config file
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/simulation-api-client-config-version.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY AnyNewerVersion
)

# Install the configuration files
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/simulation-api-client-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/simulation-api-client-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/simulation-api-client
)

install(TARGETS simulation-api-client
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
include(CPack)