# Server Library
project(vgtdsl)
set(ACTUAL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/db/DatabaseImplSqlite.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/db/DatabaseImpl.cpp
        AuthManagerImpl.cpp
    )
set(HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/db/ 
   )

include_directories(
    ${WARFTPD_ROOT_DIR}/include/warftpd/warftpd.h
    ${WARFTPD_ROOT_DIR}/include/warftpd/Database.h
    ${CMAKE_CURRENT_SOURCE_DIR}/db/DatabaseImpl.h
    )

if (WIN32)
    ADD_MSVC_PRECOMPILED_HEADER(${WARFTPD_ROOT_DIR}/include/warftpd/warftpd.h pch.cpp ACTUAL_SOURCES)
    add_definitions(-DWARFTPD_PCH)
    set(SOURCES ${ACTUAL_SOURCES} pch.cpp ${HEADERS} ${RESFILES})
else()
    set(SOURCES ${ACTUAL_SOURCES})
    set(PTHERAD ptherad)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(libwarftpd ${SOURCES})
set_target_properties(libwarftpd PROPERTIES DEBUG_OUTPUT_NAME warftpdD)
target_link_libraries(libwarftpd ${DEFAULT_LIBRARIES} sqlpp11-connector-sqlite3 sqlite3 wfde)
