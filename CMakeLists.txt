cmake_minimum_required(VERSION 3.12)

project(Function2 VERSION 0.2.0)

add_library(function2 INTERFACE)
add_library(Function2::function2 ALIAS function2)
target_include_directories(function2
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features(function2 INTERFACE cxx_std_17)

option(FUNCTION2_BUILD_TESTS "Build tests for Function2." OFF)
if(FUNCTION2_BUILD_TESTS)
    enable_testing()

    find_package(Catch2 REQUIRED)

    add_executable(test_fn2 test/runner.cpp test/fn2.spec.cpp)
    target_include_directories(test_fn2
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(test_fn2 PRIVATE Catch2::Catch2 function2)

    include(CTest)
    include(Catch)
    catch_discover_tests(test_fn2)
endif()

option(FUNCTION2_BUILD_DOCS "Build docs for Function2." OFF)
if(FUNCTION2_BUILD_DOCS)
    set(DOXYGEN_SKIP_DOT ON)
    find_package(Doxygen REQUIRED)

    set(DOXYGEN_IN "${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in")
    set(DOXYGEN_OUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

    configure_file("${DOXYGEN_IN}" "${DOXYGEN_OUT}" @ONLY)

    add_custom_target(doc ALL "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_OUT}"
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)

    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc/" DESTINATION "share/doc/${PROJECT_NAME}")
endif()

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Function2)

install(TARGETS function2
    EXPORT function2-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set_target_properties(function2 PROPERTIES EXPORT_NAME Function2)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT function2-targets
  FILE
    Function2Targets.cmake
  NAMESPACE
    Function2::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/Function2ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/Function2Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/Function2Config.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

#Install the config, configversion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/Function2Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/Function2ConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT function2-targets
       FILE ${CMAKE_CURRENT_BINARY_DIR}/Function2Targets.cmake
       NAMESPACE Function2::)

#Register package in user's package registry
export(PACKAGE Function2)
