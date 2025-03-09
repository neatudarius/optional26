# CMakeLists.txt                                                    -*-CMake-*-
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.27)

project(beman_optional VERSION 0.0.0 LANGUAGES CXX)

# Includes
include(CTest)
include(FetchContent)

set(TARGETS_EXPORT_NAME ${CMAKE_PROJECT_NAME}Targets)

option(
    OPTIONAL_ENABLE_TESTING
    "Enable building tests and test infrastructure"
    ${PROJECT_IS_TOP_LEVEL}
)

set(CMAKE_VERIFY_INTERFACE_HEADER_SETS ON)

# Create the library target and named header set for beman_optional
add_library(beman_optional INTERFACE)
target_sources(
    beman_optional
    PUBLIC FILE_SET beman_optional_headers TYPE HEADERS BASE_DIRS include
)

if(OPTIONAL_ENABLE_TESTING)
    # Create the library target and named header set for testing beman_optional
    # and mark the set private
    add_executable(beman_optional_test)
    target_sources(
        beman_optional_test
        PRIVATE
            FILE_SET beman_optional_test_headers
            TYPE HEADERS
            BASE_DIRS tests
    )

    add_subdirectory(tests/beman/optional)
endif()

add_subdirectory(include/beman/optional)

add_subdirectory(examples)

include(CMakePackageConfigHelpers)

# This will be used to replace @PACKAGE_cmakeModulesDir@
set(cmakeModulesDir cmake/beman)
configure_package_config_file(
    cmake/Config.cmake.in
    BemanOptionalConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/beman/optional/
    PATH_VARS cmakeModulesDir
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/BemanOptionalConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/beman/optional/
    COMPONENT beman_optional_development
)

# Coverage
configure_file("cmake/gcovr.cfg.in" gcovr.cfg @ONLY)

add_custom_target(
    process_coverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running gcovr to process coverage results"
    COMMAND mkdir -p coverage
    COMMAND gcovr --config gcovr.cfg .
)
