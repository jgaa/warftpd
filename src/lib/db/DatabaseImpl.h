#pragma once
#include <array>
#include <boost/filesystem.hpp>
#include "warftpd/warftpd.h"
#include "warftpd/Database.h"
#include "log/WarLog.h"
#include "war_error_handling.h"
#include "war_uuid.h"

#include "sqlpp11/sqlpp11.h"
#include <sqlpp11/ppgen.h>

using namespace std;
using namespace war;
using namespace war::wfde;

SQLPP_DECLARE_TABLE(
    (server)
    ,
    (id,    varchar(38), SQLPP_NOT_NULL)
    (name,  varchar(255), SQLPP_NOT_NULL)
    (enabled, bool, SQLPP_NOT_NULL)
)

SQLPP_DECLARE_TABLE(
    (host)
    ,
    (id,    varchar(38), SQLPP_NOT_NULL)
    (parent,varchar(38), SQLPP_NOT_NULL)
    (name,  varchar(255), SQLPP_NOT_NULL)
    (long_name, varchar(255), SQLPP_NOT_NULL)
    (enabled, bool, SQLPP_NOT_NULL)
)

SQLPP_DECLARE_TABLE(
    (protocol)
    ,
    (id,    varchar(38), SQLPP_NOT_NULL)
    (parent,varchar(38), SQLPP_NOT_NULL)
    (name,  varchar(16), SQLPP_NOT_NULL)
    (enabled, bool, SQLPP_NOT_NULL)
)

SQLPP_DECLARE_TABLE(
    (interface)
    ,
    (id,    varchar(38), SQLPP_NOT_NULL)
    (parent,varchar(38), SQLPP_NOT_NULL)
    (name,  varchar(255), SQLPP_NOT_NULL)
    (hostname, varchar(255), SQLPP_NOT_NULL)
    (port, varchar(255), SQLPP_NOT_NULL)
    (enabled, bool, SQLPP_NOT_NULL)
)

SQLPP_DECLARE_TABLE(
    (permission)
    ,
    (id,    varchar(38), SQLPP_NOT_NULL)
    (parent,varchar(38), SQLPP_NOT_NULL)
    (type,  int, SQLPP_NOT_NULL)
)

SQLPP_DECLARE_TABLE(
    (path)
    ,
    (id,    varchar(38), SQLPP_NOT_NULL)
    (permission_id,varchar(38), SQLPP_NOT_NULL)
    (vpath, varchar(255), SQLPP_NOT_NULL)
    (ppath, varchar(255), SQLPP_NOT_NULL)
    (permissions, bigint, SQLPP_NOT_NULL)
)

SQLPP_DECLARE_TABLE(
    (user)
    ,
    (id,    varchar(38), SQLPP_NOT_NULL)
    (parent, varchar(38), SQLPP_NOT_NULL)
    (login_name, varchar(255), SQLPP_NOT_NULL)
    (full_name, varchar(255), SQLPP_NOT_NULL)
    (auth_type,  int, SQLPP_NOT_NULL)
    (passwd, varchar(255), SQLPP_NOT_NULL)
    (enabled, bool, SQLPP_NOT_NULL)
)

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
    const string all{"%"};
    
    DatabaseImpl(const war::wfde::Configuration& conf)
    : conn_{conf}
    {}
    
    UserData FindUser(const boost::uuids::uuid parent, 
                      const string& loginName) override {
        user::user user;
        
        auto result = GetDb()(select(
            user.login_name, user.auth_type, user.passwd, user.id).from(user).where(
            user.parent.like(boost::uuids::to_string(parent))
            and user.login_name.like(loginName)
            and user.enabled));
        
        auto row = result.begin();
        if (row == result.end()) {
            LOG_DEBUG << "User " << log::Esc(loginName) 
                << " was not found or is inactive.";
            WAR_THROW_T(ExceptionNotFound, "User don't exist or is inactive");
        }
        
        UserData ud;
        
        if (row->auth_type == static_cast<int>(AuthTypes::CLEAR_PASSWD)) {
            ud.auth_type = AuthTypes::CLEAR_PASSWD;
        } else {
            WAR_THROW_T(ExceptionOutOfRange, "Invalid auth_type");
        }
        
        ud.id = get_uuid_from_string(row->id);
        ud.passwd = row->passwd;
        ud.login_name = row->login_name;
        
        return ud;
    }
    
    template <typename rowT>
    Configuration::ptr_t Configure(const rowT& row)
    {
        auto conf = Configuration::CreateInstance();
        conf->SetValue("/Id", row.id);
        conf->SetValue("/Name", row.name);
        conf->SetValue("/Enabled", row.enabled ? "1" : "0");
        return conf;
    }
    
    template <typename rowT>
    Configuration::ptr_t&& Configure(Configuration::ptr_t&& conf, const rowT& row) {
        conf->SetValue("/Parent", row.parent);
        return move(conf);
    }
   
    ObjectList FindServer(const string& key = "") override {
        ObjectList rval;
        rval.type = Entity::Type::SERVER;
        server::server d;
        for (const auto& row : GetDb()(
            select(all_of(d)).from(d).where(d.name.like(key.empty() ? all : key)))) {
            rval.conf.push_back(Configure(row));
        }
        
        return rval;
    }
    
    ObjectList FindHost(const Server& parent, const string& key = "") override {
        ObjectList rval;
        rval.type = Entity::Type::HOST;
        host::host d;
        for (const auto& row : GetDb()(select(all_of(d)).from(d).where(
            d.parent.like(boost::uuids::to_string(parent.GetId())) 
            and d.name.like(key.empty() ? all : key)))) {
            
            auto c = Configure(Configure(row), row);
            rval.conf.push_back(move(c));
        }
        
        return rval;
    }
    
    ObjectList FindProtocol(const Host& parent, const string& key = "") override {
        ObjectList rval;
        rval.type = Entity::Type::PROTOCOL;
        protocol::protocol d;
        for (const auto& row : GetDb()(select(all_of(d)).from(d).where(
            d.parent.like(boost::uuids::to_string(parent.GetId())) 
            and d.name.like(key.empty() ? all : key)))) {
            
            auto c = Configure(Configure(row), row);
            rval.conf.push_back(move(c));
        }
       
        return rval;
    }
    
    ObjectList FindInterface(const Protocol& parent, const string& key = "") override {
        ObjectList rval;
        rval.type = Entity::Type::INTERFACE;
        interface::interface d;
        for (const auto& row : GetDb()(select(all_of(d)).from(d).where(
            d.parent.like(boost::uuids::to_string(parent.GetId())) 
            and d.name.like(key.empty() ? all : key)))) {
            
            auto c = Configure(Configure(row), row);
            c->SetValue("/Ip", row.hostname);
            c->SetValue("/Port", row.port);
            rval.conf.push_back(move(c));
        }
            
        return rval;
    }
    
    
    void Bootstrap() override {
        // Read the create table script
        boost::filesystem::path path = "sql";
        path /= conn_.GetCreateDbScriptName();
        
        // Create the tables. Any existing data is lost.
        conn_.ExecuteScript(path.string());
        
        // Add data to create a skeleton setup on localhost.
        const string server_id = get_uuid_as_string();
        const string host_id = get_uuid_as_string();
        const string ftp_id = get_uuid_as_string();
        
        {
            server::server svr;
            GetDb()(insert_into(svr).set(
                svr.id = server_id, 
                svr.name = "Server",
                svr.enabled = true));
        }
        
        {
            host::host host;
            GetDb()(insert_into(host).set(
                host.id = host_id,
                host.parent = server_id, 
                host.name = "FanClub",
                host.long_name = WFDE_DEFAULT_HOST_LONG_NAME,
                host.enabled = true));
        }
        
        {
            protocol::protocol prot;
            GetDb()(insert_into(prot).set(
                prot.id = ftp_id,
                prot.parent = host_id, 
                prot.name = "FTP",
                prot.enabled = true));
        }
        
        {
            interface::interface inf;
            GetDb()(insert_into(inf).set(
                inf.id = get_uuid_as_string(),
                inf.parent = ftp_id, 
                inf.name = "TCP Localhost",
                inf.hostname = "127.0.0.1",
                inf.port = "2121",
                inf.enabled = true));
        }
        
        {
            user::user user;
            GetDb()(insert_into(user).set(
                user.id = get_uuid_as_string(),
                user.parent = "", // No parent - this user has access to all objects
                user.login_name = "admin",
                user.full_name = "Administrator",
                user.auth_type = static_cast<int>(AuthTypes::CLEAR_PASSWD),
                user.passwd = "secret", // TODO: Change to hashed type with unique passwd
                user.enabled = true));
            
            GetDb()(insert_into(user).set(
                user.id = get_uuid_as_string(),
                user.parent = host_id, 
                user.login_name = "demo",
                user.full_name = "Demo User",
                user.auth_type = static_cast<int>(AuthTypes::CLEAR_PASSWD),
                user.passwd = "secret", // TODO: Change to hashed type with unique passwd
                user.enabled = true));
            
            GetDb()(insert_into(user).set(
                user.id = get_uuid_as_string(),
                user.parent = host_id, 
                user.login_name = "anonymous",
                user.full_name = "Anonymous user",
                user.auth_type = static_cast<int>(AuthTypes::ANONYMOUS_USER),
                user.passwd = "", 
                user.enabled = true));
        }
    }

protected:
    db_t& GetDb() { return conn_.GetDb(); }
    
    
private:
    conn_t conn_;
};

    
}// impl
} //warftpd

