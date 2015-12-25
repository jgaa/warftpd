
#include "warftpd/warftpd.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "DatabaseImpl.h"
#include <sqlpp11/sqlite3/sqlite3.h>
#include "log/WarLog.h"

namespace sql = sqlpp::sqlite3;

namespace warftpd {
namespace impl {

class SqliteConnection
{
public:
    using db_t = sqlpp::sqlite3::connection;
    using conf_t = sqlpp::sqlite3::connection_config;
    
    SqliteConnection(const war::wfde::Configuration& conf) 
    : db_{GetConf(conf)}
    {
        LOG_DEBUG_FN << "Using sqlite3 database.";
    }
    
    db_t& GetDb() {
        return db_;
    }
    
    const std::string GetCreateDbScriptName() const {
        return "create_db-sqlite.sql";
    }
    
    void ExecuteScript(const string& path)
    {
        LOG_NOTICE_FN << "Preparing to execute the following SQL script: "
        << log::Esc(path);
        
        ifstream sql_file(path.c_str());
        std::string sql;
        sql.assign(istreambuf_iterator<char>(sql_file),
                   istreambuf_iterator<char>());
        
        db_.execute("begin");
        try {
            std::vector<std::string> statements;
            boost::split(statements, sql, boost::is_any_of(";"));
            
            for(const auto& statement: statements) {
                if (!IsEmptyStatement(statement)) {
                    LOG_TRACE1_FN << "Executing the following SQL statement: "
                        << log::Esc(statement);
                    db_.execute(statement);
                }
            }
        } catch (const exception& ex) {
            LOG_ERROR_FN << "Caught exception: " << ex;
            db_.execute("rollback");
            throw;
        }
        db_.execute("commit");
    }
    
    
private:
    bool IsEmptyStatement(const std::string& statement) const {
        for(auto ch : statement) {
            if (!isspace(ch, loc_)) {
                return false;
            }
        }
        
        return true;
    }
    
    conf_t GetConf(const war::wfde::Configuration& conf) {
        conf_t sqlite_conf;
        
        sqlite_conf.path_to_database = conf.GetValue("Database/Path", "warftpd.db");
        sqlite_conf.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        
        return sqlite_conf;
    }
    
    db_t db_;
    std::locale loc_;
};

class DatabaseImplSqlite : public DatabaseImpl<SqliteConnection>
{
public:
    using db_t = sqlpp::sqlite3::connection;
    using conf_t = sqlpp::sqlite3::connection_config;
    
    
    DatabaseImplSqlite(const war::wfde::Configuration& conf)
    : DatabaseImpl(conf)
    {
    }
    
private:
   
};
    
Database::ptr_t CreateSqliteDbInstance(const war::wfde::Configuration& conf) {
    
    return std::make_unique<DatabaseImplSqlite>(conf);
}

} //impl

} // warftpd
