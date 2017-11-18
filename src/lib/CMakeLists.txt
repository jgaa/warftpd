project(libwarftpd VERSION 4.0 LANGUAGES CXX)
set(ACTUAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/db/DatabaseImplSqlite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/db/DatabaseImpl.cpp
    AuthManagerImpl.cpp
    )
set(HEADERS
    ${WARFTPD_ROOT_DIR}/include/warftpd/warftpd.h
    ${WARFTPD_ROOT_DIR}/include/warftpd/Database.h
    ${CMAKE_CURRENT_SOURCE_DIR}/db/DatabaseImpl.h
   )

if (WIN32)
    set(SOURCES ${ACTUAL_SOURCES} pch.cpp ${HEADERS} ${RESFILES})
else()
    set(SOURCES ${ACTUAL_SOURCES})
endif()

add_library(libwarftpd ${SOURCES})
add_dependencies(libwarftpd externalWarlib externalWfde externalSqlpp11 externalSqlpp11Sqlite3)
set_property(TARGET libwarftpd PROPERTY CXX_STANDARD 14)
target_include_directories(libwarftpd
    PUBLIC $<BUILD_INTERFACE:${WARFTPD_ROOT}/include> $<INSTALL_INTERFACE:include>
    PRIVATE ${CMAKE_BINARY_DIR}/generated-include/
    PRIVATE ${WARFTPD_ROOT}/src/libwarftpd
    PRIVATE ${EXTERNAL_PROJECTS_PREFIX}/src/externalHinnatDate/include/date
    PRIVATE PRIVATE ${SQLITE3_INCLUDE_DIRS}
    )
target_link_libraries(libwarftpd PUBLIC wfde ${SQLITE3_LIBRARY})
target_compile_definitions(${PROJECT_NAME} PUBLIC -DBOOST_COROUTINES_NO_DEPRECATION_WARNING=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DBOOST_COROUTINE_NO_DEPRECATION_WARNING=1)
