project(warftpd VERSION 0.21.0 LANGUAGES CXX)

set(ACTUAL_SOURCES
    main.cpp
    DaemonImpl.cpp
    )

set(HEADERS
    ${WARFTPD_ROOT_DIR}/include/warftpd/warftpd.h
    )

if (WIN32)
    set(FTPD_SOURCES ${ACTUAL_SOURCES} ${HEADERS} ${RESFILES})
else()
    set(FTPD_SOURCES ${ACTUAL_SOURCES})
endif()

add_executable(warftpd ${FTPD_SOURCES})
set_property(TARGET warftpd PROPERTY CXX_STANDARD 14)
target_link_libraries(warftpd PRIVATE libwarftpd wfde warcore
    ${Boost_LIBRARIES}
    ${externalSqlpp11Sqlite3_LIBRARIES}
    sqlpp11-connector-sqlite3
    ${SQLITE3_LIBRARIES}
    ${OPENSSL_LIBRARIES}
#     ${_LIBRARIES}
#     ${_LIBRARIES}
#     ${_LIBRARIES}
    )
target_compile_definitions(${PROJECT_NAME} PRIVATE -DBOOST_COROUTINES_NO_DEPRECATION_WARNING=1)
target_compile_definitions(${PROJECT_NAME} PRIVATE -DBOOST_COROUTINE_NO_DEPRECATION_WARNING=1)
#add_dependencies(warftpd libwarftpd externalWarlib externalWfde externalSqlpp11 externalSqlpp11Sqlite3 )

target_include_directories(warftpd
    PRIVATE ${WARFTPD_ROOT}/include
    PRIVATE ${CMAKE_BINARY_DIR}/generated-include
    PRIVATE ${WARFTPD_ROOT}/src/daemon
)

install(TARGETS DESTINATION ${CMAKE_INSTALL_BINDIR})

