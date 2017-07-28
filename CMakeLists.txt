# Defines the CMake commands/policies
cmake_minimum_required( VERSION 2.8.5 )
project(CellLists CXX)

# Version information
set(CELLLISTS_MAJOR 0)
set(CELLLISTS_MINOR 0)
set(CELLLISTS_PATCH 0b1)


# The googletest snippets were taken from:
#    http://crascit.com/2015/07/25/cmake-gtest/
# and modified to work with googletest-1.8.0 instead of the latest git revision.

# Download and unpack googletest at configure time
configure_file(CMakeListsGTest.txt.in
               googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
execute_process(COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)

# Add googletest directly to our build. This adds the following targets: gtest,
# gtest_main, gmock and gmock_main
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL )

# NOTE: A change request has been submitted to the GoogleTest
#       project to remove the need for the following lines.
#       They add the gtest and gmock include directories as
#       target dependencies, since they are not added by
#       googletest (yet).
target_include_directories(gtest INTERFACE
    "${CMAKE_BINARY_DIR}/googletest-src/include")
target_include_directories(gtest_main INTERFACE
    "${CMAKE_BINARY_DIR}/googletest-src/include")

# # Only switch this one when done with googletest.
# set(CMAKE_VERBOSE_MAKEFILE "ON")

# General compiler flags
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wconversion -Wextra")

# Additional compiler settings when doing a debug build
string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    # These settings are only useful in dev mode.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -pedantic")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # When coding (debug mode), we want to know when there might be trouble ...
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -coverage")
        set(CELLLISTS_DO_COVERAGE "ON")
        # Google test uses comparisons between signed and unsigned ...
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
    endif()
endif()

add_subdirectory( celllists )

# Documentation with doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${PROJECT_SOURCE_DIR}/doxygen.conf.in ${PROJECT_BINARY_DIR}/doxygen.conf @ONLY)
    add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/doxygen.conf
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)
