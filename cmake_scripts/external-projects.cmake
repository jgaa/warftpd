# Here are registered all external projects
#
# Usage:
# add_dependencies(TARGET externalProjectName)
# target_link_libraries(TARGET PRIVATE ExternalLibraryName)

set(EXTERNAL_PROJECTS_PREFIX ${CMAKE_BINARY_DIR}/external-projects)
set(EXTERNAL_PROJECTS_INSTALL_PREFIX ${EXTERNAL_PROJECTS_PREFIX}/installed)

include(GNUInstallDirs)

# MUST be called before any add_executable() # https://stackoverflow.com/a/40554704/8766845
link_directories(${EXTERNAL_PROJECTS_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
include_directories($<BUILD_INTERFACE:${EXTERNAL_PROJECTS_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}>)

include(ExternalProject)

ExternalProject_Add(externalHinnatDate
    PREFIX "${EXTERNAL_PROJECTS_PREFIX}"
    GIT_REPOSITORY "https://github.com/HowardHinnant/date.git"
    GIT_TAG "master"
    TEST_BEFORE_INSTALL 0
    TEST_AFTER_INSTALL 0
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${EXTERNAL_PROJECTS_PREFIX}/src/externalHinnatDate/include ${EXTERNAL_PROJECTS_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}
    )

set(externalHinnatDate_INCLUDE_DIR "${EXTERNAL_PROJECTS_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/date")

ExternalProject_Add(externalSqlpp11
    PREFIX "${EXTERNAL_PROJECTS_PREFIX}"
    GIT_REPOSITORY "https://github.com/rbock/sqlpp11.git"
    GIT_TAG "master"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_PROJECTS_INSTALL_PREFIX} -DHinnantDate_ROOT_DIR=${EXTERNAL_PROJECTS_PREFIX}/src/externalHinnatDate -DHinnantDate_INCLUDE_DIR=${externalHinnatDate_INCLUDE_DIR} -DENABLE_TESTS=0
    TEST_BEFORE_INSTALL 0
    TEST_AFTER_INSTALL 0
    DEPENDS externalHinnatDate
    )

ExternalProject_Add(externalSqlpp11Sqlite3
    PREFIX "${EXTERNAL_PROJECTS_PREFIX}"
    GIT_REPOSITORY "https://github.com/rbock/sqlpp11-connector-sqlite3.git"
    GIT_TAG "master"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_PROJECTS_INSTALL_PREFIX} -DHinnantDate_ROOT_DIR=${EXTERNAL_PROJECTS_PREFIX}/src/externalHinnatDate -DHinnantDate_INCLUDE_DIR=${externalHinnatDate_INCLUDE_DIR} -DENABLE_TESTS=0 -DSQLPP11_INCLUDE_DIR=${EXTERNAL_PROJECTS_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}
    TEST_BEFORE_INSTALL 0
    TEST_AFTER_INSTALL 0
    DEPENDS externalSqlpp11 externalHinnatDate
    )

ExternalProject_Add(externalWarlib
    PREFIX "${EXTERNAL_PROJECTS_PREFIX}"
    GIT_REPOSITORY "https://github.com/jgaa/warlib.git"
    GIT_TAG "master"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_PROJECTS_INSTALL_PREFIX} -DWAR_AUTORUN_UNIT_TESTS=0
    TEST_BEFORE_INSTALL 0
    TEST_AFTER_INSTALL 0
    )

ExternalProject_Add(externalWfde
    PREFIX "${EXTERNAL_PROJECTS_PREFIX}"
    GIT_REPOSITORY "https://github.com/jgaa/wfde.git"
    GIT_TAG "master"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_PROJECTS_INSTALL_PREFIX} -DWFDE_AUTORUN_UNIT_TESTS=0 -DWFDE_WITH_TLS=${WARFTPD_WITH_TLS}
    TEST_BEFORE_INSTALL 0
    TEST_AFTER_INSTALL 0
    DEPENDS externalWarlib
    )

ExternalProject_Add(externalLest
    PREFIX "${EXTERNAL_PROJECTS_PREFIX}"
    GIT_REPOSITORY "https://github.com/martinmoene/lest.git"
    GIT_TAG "master"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_PROJECTS_INSTALL_PREFIX} -DLEST_BUILD_EXAMPLE=OFF
    TEST_BEFORE_INSTALL 0
    TEST_AFTER_INSTALL 0
    )
