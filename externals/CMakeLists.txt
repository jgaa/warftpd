# Here we build libraries from external sources.

add_library(sqlpp11-connector-sqlite3
        ${SQLPP_SQLITE_ROOT_DIR}/src/connection.cpp
        ${SQLPP_SQLITE_ROOT_DIR}/src/bind_result.cpp
        ${SQLPP_SQLITE_ROOT_DIR}/src/prepared_statement.cpp
        ${SQLPP_SQLITE_ROOT_DIR}/src/detail/connection_handle.cpp)
set_target_properties(sqlpp11-connector-sqlite3 PROPERTIES DEBUG_OUTPUT_NAME sqlpp11-connector-sqlite3D)

