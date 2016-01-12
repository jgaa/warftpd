#pragma once

#include "warftpd.h"

namespace warftpd {
 
/*! Database interface for warftpd.
 */

class Database
{
public:
    using ptr_t = std::shared_ptr<Database>;
    using id_t = uint32_t;
    
    struct ObjectList {
        war::wfde::Entity::Type type;
        std::deque<war::wfde::Configuration::ptr_t> conf;
    };
    
    Database() = default;
    virtual ~Database() = default;
    Database(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator = (const Database&) = delete;
    Database& operator = (Database&&) = delete;
        
    struct UserData {
        std::string login_name;
        AuthTypes auth_type;
        std::string passwd;
        boost::uuids::uuid id;
        
        ~UserData() {
            // Clear the data in the password.
            for(auto& ch : passwd) {
                ch = ' ';
            }
            passwd.clear();
        }
    };
    
    /*! Lookup a user in the database */
    virtual UserData FindUser(const boost::uuids::uuid parent, 
                              const std::string& loginName) = 0;
    
    /*! Search for the key.
     * 
     * @param Key to search for. May contain SQL wild-chard.
     *      An empty string matches everything.
     * 
     * @return All the nodes that matches the key.
     */
    virtual ObjectList
    FindServer(const std::string& key = "") = 0;
    
    /*! Search for the key.
     * 
     * @param Key to search for. May contain SQL wild-chard.
     * 
     * @return All the nodes that matches the key.
     */
    virtual ObjectList
    FindHost(const war::wfde::Server& parent, const std::string& key = "") = 0;
    
    /*! Search for the key.
     * 
     * @param Key to search for. May contain SQL wild-chard.
     *      An empty string matches everything.
     * 
     * @return All the nodes that matches the key.
     */
    virtual ObjectList
    FindProtocol(const war::wfde::Host& parent, const std::string& key = "") = 0;
    
    /*! Search for the key.
     * 
     * @param Key to search for. May contain SQL wild-chard.
     *      An empty string matches everything.
     * 
     * @return All the nodes that matches the key.
     */
    virtual ObjectList
    FindInterface(const war::wfde::Protocol& parent, const std::string& key = "") = 0;
    
    /*! Get the permissions for an object.
     * 
     * The permissions are the ones defined for this object. They are
     * returned as is, without being merged into effective permissions.
     * 
     * @returns Pointer to Configuration data or nullpointer
     */
    virtual war::wfde::Configuration::ptr_t
    GetPermissions(const war::wfde::Entity& node) = 0;
    
    /*! Get the permissions for a user
     * 
     * The permissions are the ones defined for this object. They are
     * returned as is, without being merged into effective permissions.
     * 
     * @returns Pointer to Configuration data or nullpointer
     */
    virtual war::wfde::Configuration::ptr_t
    GetPermissions(const war::wfde::Client& client) = 0;
    
    /*! Bootstrap the database
     * 
     * This erases any existing data and resets the database.
     * 
     * This method is meant to be run when the server is installed.
     * 
     * For databases with authentication (like postgresql), the database must exist.
     */
    virtual void  Bootstrap() = 0;
    
    /*! Create an instance of the Database object. 
     * 
     * The database is opened. If a database connection pool is  supported by the
     * database driver selected by the configuration, it is initialized under the
     * hood.
     */    
    static Database::ptr_t CreateInstance(const war::wfde::Configuration& conf);
};

}
