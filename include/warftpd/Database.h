#pragma once

#include "warftpd.h"

namespace warftpd {
 
/*! Database interface for warftpd.
 */

class Database
{
public:
    using ptr_t = std::shared_ptr<Database>;
    using id_t = boost::uuids::uuid;
    
    template <typename T>
    struct ObjectList {
        std::deque<typename T::ptr_t> object;
    };
    
    Database() = default;
    virtual ~Database() = default;
    Database(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator = (const Database&) = delete;
    Database& operator = (Database&&) = delete;
    
    war::wfde::Server::ptr_t GetServer(const id_t& id);
    war::wfde::Host::ptr_t GetHost(const id_t& id);
    war::wfde::Protocol::ptr_t GetProtocol(const id_t& id);
    war::wfde::Interface::ptr_t GetInterface(const id_t& id);
    
    /*! Search for the key.
     * 
     * @param Key to search for. May contain SQL wild-chard.
     *      An empty string matches everything.
     * 
     * @return All the nodes that matches the key.
     */
    virtual ObjectList<war::wfde::Server> 
    FindServer(const std::string& key = "") = 0;
    
    /*! Search for the key.
     * 
     * @param Key to search for. May contain SQL wild-chard.
     * 
     * @return All the nodes that matches the key.
     */
    virtual ObjectList<war::wfde::Host> 
    FindHost(const war::wfde::Server& parent, const std::string& key = "") = 0;
    
    /*! Search for the key.
     * 
     * @param Key to search for. May contain SQL wild-chard.
     *      An empty string matches everything.
     * 
     * @return All the nodes that matches the key.
     */
    virtual ObjectList<war::wfde::Protocol> 
    FindProtocol(const war::wfde::Host& parent, const std::string& key = "") = 0;
    
    /*! Search for the key.
     * 
     * @param Key to search for. May contain SQL wild-chard.
     *      An empty string matches everything.
     * 
     * @return All the nodes that matches the key.
     */
    virtual ObjectList<war::wfde::Interface> 
    FindInterface(const war::wfde::Protocol& parent, const std::string& key = "") = 0;
    
    /*! Load all nodes relevant to the supplied entity. 
     * 
     * @param parent. Load all nodes relevant to this entity. If the value is nullptr,
     *      the entire node-tree, staring with the Server(s) are returned.
     */
    
    virtual ObjectList<war::wfde::Entity> 
    LoadAll(const war::wfde::Entity *parent = nullptr) = 0;
    
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
