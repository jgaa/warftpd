# Main project definition for warftpd

cmake_minimum_required(VERSION 3.0)
project (warftpd)

enable_testing()

if (NOT DEFINED WITH_APIDOC)
	option(WITH_APIDOC "Generate Doxygen documentation")
endif()

if (NOT DEFINED WITH_UNITTESTS)
    option(WITH_UNITTESTS "Enable unittests" ON)
endif()

if (NOT DEFINED WARFTPD_ROOT_DIR)
    set(WARFTPD_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if (NOT DEFINED WARLIB_ROOT_DIR)
    set(WARLIB_ROOT_DIR ${WARFTPD_ROOT_DIR}/externals/warlib)
endif()

if (NOT DEFINED WFDE_ROOT_DIR)
    set(WFDE_ROOT_DIR ${WARFTPD_ROOT_DIR}/externals/wfde)
endif()

if (NOT DEFINED SQLPP_ROOT_DIR)
    set(SQLPP_ROOT_DIR ${WARFTPD_ROOT_DIR}/externals/sqlpp11)
endif()

# Tell warlib's cmake setup that it's embedded into a higher level project
set(EMBEDDED_WARLIB ON)
set(EMBEDDED_WFDELIB ON)

find_package(OpenSSL REQUIRED)

message(STATUS "WARLIB_ROOT_DIR: ${WARLIB_ROOT_DIR}")
include(${WARLIB_ROOT_DIR}/cmake_scripts/boost.cmake)
include(${WARLIB_ROOT_DIR}/cmake_scripts/pch.cmake)

set(DEFAULT_LIBRARIES ${DEFAULT_LIBRARIES}
    war_tasks
    war_log
    war_base
    )

# We create a configuration file so that other code that
# include our header files gets the correct configuration.
set(CONF_PATH ${PROJECT_SOURCE_DIR}/include/warftpd/warftpd_config.h)

message(STATUS "Using ${CMAKE_CXX_COMPILER}")

if (UNIX)
  # For now, assume we use g++/clang++
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG=1 -o3 ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1 -D_DEBUG=1 -o0 -g ")
    add_definitions(-D__USE_LARGEFILE64=1 -D__USE_FILE_OFFSET64=1 -Wall -fPIC -std=c++1y -pthread)
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG=1 ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1 -D_DEBUG=1 ")
    # We will support windows from Windows Vista
    add_definitions(-D_WIN32_WINNT=0x0600)
endif()

include_directories(${Boost_INCLUDE_DIRS})

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib CACHE PATH "Destination location")
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${WARLIB_ROOT_DIR}/include
    ${WFDE_ROOT_DIR}/include
    ${SQLPP_ROOT_DIR}/include
    )

add_subdirectory(${WARLIB_ROOT_DIR})
add_subdirectory(${WFDE_ROOT_DIR})
add_subdirectory(src)

if (WITH_APIDOC)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)

        set(DOXYGEN_INPUT Doxygen)
        set(DOXYGEN_OUTPUT doc/html)

        add_custom_command(
            OUTPUT ${DOXYGEN_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${DOXYGEN_INPUT}
        )

        add_custom_target(vgtd_apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})

    endif (DOXYGEN_FOUND)
endif()

message(STATUS "Writing the current vgtd configuration to ${CONF_PATH}")
CONFIGURE_FILE(warftpd_config.h.template ${CONF_PATH})

message(STATUS "Binary files will be located here: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

