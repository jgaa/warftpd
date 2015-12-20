project(simpleftpd)

set(FTPD_ACTUAL_SOURCES
    main.cpp
    DaemonImpl.cpp
    )

set(HEADERS
    ${WARFTPD_ROOT_DIR}/include/warftpd/warftpd.h
    )

if (WIN32)
    ADD_MSVC_PRECOMPILED_HEADER(warftpd.h pch.cpp FTPD_ACTUAL_SOURCES)
    add_definitions(-DWAR_PCH)
    set(FTPD_SOURCES ${FTPD_ACTUAL_SOURCES} pch.cpp ${HEADERS} ${RESFILES})
else()
    set(FTPD_SOURCES ${FTPD_ACTUAL_SOURCES})
endif()

add_executable(warftpd ${FTPD_SOURCES})
set_target_properties(warftpd PROPERTIES DEBUG_OUTPUT_NAME warftpdD)
target_link_libraries(warftpd ${DEFAULT_LIBRARIES} wfde)

