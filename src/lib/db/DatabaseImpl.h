#pragma once

#include <boost/filesystem.hpp>
#include "warftpd/warftpd.h"
#include "warftpd/Database.h"
#include "log/WarLog.h"
#include "war_error_handling.h"

#include "sqlpp11/sqlpp11.h"

using namespace std;
using namespace war;
using namespace war::wfde;

namespace warftpd {
namespace impl {    

/* Experimental implementation of the database to get some experience with sqlpp11.
 * 
 * We will use sqlite for the initial code. Then we will support postgresql.
 */
template<typename T>
class DatabaseImpl : public Database {
public:
    
    using db_t = typename T::db_t;
    using conn_t = T;
    
    DatabaseImpl(const war::wfde::Configuration& conf)
    : conn_(conf)
    {}
   
    ObjectList<Server> FindServer(const string& key = "") override {
        WAR_THROW_T(ExceptionNotImplemented, WAR_FUNCTION_NAME);
    }
    
    ObjectList<Host> FindHost(const Server& parent, const string& key = "") override {
        WAR_THROW_T(ExceptionNotImplemented, WAR_FUNCTION_NAME);
    }
    
    ObjectList<Protocol> FindProtocol(const Host& parent, const string& key = "") override {
        WAR_THROW_T(ExceptionNotImplemented, WAR_FUNCTION_NAME);
    }
    
    ObjectList<Interface> FindInterface(const Protocol& parent, const string& key = "") override {
        WAR_THROW_T(ExceptionNotImplemented, WAR_FUNCTION_NAME);
    }
    
    ObjectList<Entity> LoadAll(const Entity* parent = nullptr) override {
        WAR_THROW_T(ExceptionNotImplemented, WAR_FUNCTION_NAME);
    }
    
    void Bootstrap() override {
        // Read the create table script
        boost::filesystem::path path = "sql";
        path /= conn_.GetCreateDbScriptName();
        
        conn_.ExecuteScript(path.string());
        
        // TODO: Fill the database with initial data
    }

protected:
    db_t& GetDb() { return conn_.GetDb(); }
    
private:
    conn_t conn_;
};

    
}// impl
} //warftpd

