#pragma once

#include "warftpd/warftpd.h"

namespace warftpd {

/*! The War FTP Daemon instance class */
class Daemon
{
public:
    using ptr_t = std::shared_ptr<Daemon>;
    
    Daemon() = default;
    virtual ~Daemon() = default;
    Daemon(const Daemon&) = delete;
    Daemon(Daemon&&) = delete;
    Daemon& operator = (const Daemon&) = delete;
    Daemon& operator = (Daemon&&) = delete;
    
    /*! Start the service. 
     * 
     * The call will return almost immediately.
     */
    virtual void Start() = 0;
    
    /*! Start shutdown of the service.
     * 
     * The call will return immediately 
     */
    virtual void Shutdown() = 0;
    
    /*! Returns when the shutdown is complete */
    virtual void WaitForServiceShutdown() = 0;
    
    /*! Load the permissions from wherever they are persisted */
    virtual Permissions::ptr_t GetPermissions(const User& user) = 0;
    
    /*! Load the permissions from wherever they are persisted */
    virtual Permissions::ptr_t GetPermissions(const Entity& entity) = 0;
    
    /*! Factory */
    static ptr_t Create(war::wfde::Configuration::ptr_t& conf);
};

} // namespace
