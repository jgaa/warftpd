cmake_minimum_required(VERSION 3.0)
project (warftpd_project VERSION 4.0 LANGUAGES CXX)

set (WARFTPD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT DEFINED WARFTPD_WITH_TLS)
    option(WARFTPD_WITH_TLS "Enable TLS" ON)
endif()

if (NOT DEFINED WITH_APIDOC)
    option(WITH_APIDOC "Generate Doxygen documentation")
endif()

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake_scripts)


# if (NOT DEFINED WARFTPD_WITH_UNIT_TESTS)
#     option(WARFTPD_WITH_UNIT_TESTS "Enable Unit Testing" ON)
#     if (NOT DEFINED WARFTPD_AUTORUN_UNIT_TESTS)
#         option(WARFTPD_AUTORUN_UNIT_TESTS "Run unit tests during build" OFF)
#     endif()
#     include(cmake_scripts/add_and_run_test.cmake)
# endif()

message(STATUS "Using ${CMAKE_CXX_COMPILER}")

include(cmake_scripts/external-projects.cmake)

find_package(SQLite3 REQUIRED)

# Include Boost as an imported target
find_package(Boost 1.50 REQUIRED COMPONENTS
    system
    program_options
    filesystem
    date_time
    context
    coroutine
    chrono
    regex
    )

# add_library(boost INTERFACE IMPORTED)
# set_property(TARGET boost PROPERTY
#     INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
#     )

if (WARFTPD_WITH_TLS)
    find_package(OpenSSL REQUIRED)
endif()

add_subdirectory(src/lib)
add_subdirectory(src/daemon)

# if (WARFTPD_WITH_UNIT_TESTS)
#     add_subdirectory(tests)
# endif()

CONFIGURE_FILE(config.h.template ${CMAKE_BINARY_DIR}/generated-include/warftpd/config.h)

