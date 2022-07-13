
# Define library target

set(LIB_HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/libbech32/bech32.h
)

set(LIB_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/bech32.cpp
)

add_library(bech32 STATIC ${LIB_HEADER_FILES} ${LIB_SOURCE_FILES})
add_library(bech32::bech32 ALIAS bech32)

set_property(
        TARGET bech32
        PROPERTY PUBLIC_HEADER "${LIB_HEADER_FILES}"
)

# Set include directory

target_include_directories(bech32
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/libbech32>
)

# Misc properties

target_compile_features(bech32 PRIVATE cxx_std_11)
set_target_properties(bech32 PROPERTIES CXX_EXTENSIONS OFF)

# Set version

target_compile_definitions(bech32 PRIVATE -DLIBBECH32_VERSION_MAJOR=${LIBBECH32_VERSION_MAJOR})
target_compile_definitions(bech32 PRIVATE -DLIBBECH32_VERSION_MINOR=${LIBBECH32_VERSION_MINOR})
target_compile_definitions(bech32 PRIVATE -DLIBBECH32_VERSION_PATCH=${LIBBECH32_VERSION_PATCH})


if(INSTALL_LIBBECH32)

    # Configuration

    set(version_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
    set(project_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
    set(namespace "bech32::")

    # Include module with function 'write_basic_package_version_file'
    include(CMakePackageConfigHelpers)

    # Configure '<PROJECT-NAME>ConfigVersion.cmake'
    # Note: PROJECT_VERSION is used as a VERSION
    write_basic_package_version_file(
            "${version_config}"
            COMPATIBILITY SameMajorVersion
    )

    # Install '<PROJECT-NAME>ConfigVersion.cmake'
    install(
            FILES ${version_config}
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/libbech32)

    # Configure/Install '<PROJECT-NAME>Config.cmake'
    install(
            EXPORT bech32
            NAMESPACE ${namespace}
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/libbech32
            FILE "${PROJECT_NAME}Config.cmake"
    )

    # Install Targets

    install(
            TARGETS bech32
            EXPORT bech32
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libbech32
    )


endif()
