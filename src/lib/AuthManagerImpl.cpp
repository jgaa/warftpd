
/* 
 * TODO: Deal with email as passwd
 * 
 *  The work-flow is as follows:
 *      When Login() is called, we lookup the name in the cache.
 *      If it is found, we may proceed and authenticate the user based on
 *      the password. 
 * 
 *      If the cached user is more than n seconds old, the password
 *      and availability will be re-loaded from the database.
 * 
 *      If the user is not cached, the data is loaded from the 
 *      database to the cache.
 * 
 *      When the instance-count of a user drops to zero, a timer is scheduled
 *      to destroy the cache entry after a few seconds. If the entry is still
 *      unused when the timer function is executed, the cache is freed of this
 *      entry.
 * 
 *      Loading properties is an expensive operation. It is executed the first
 *      time GetPermissions() is called, and whenever the permissions is more than
 *      n seconds old (to allow updates to propagate from the database to the 
 *      cache for long-running sessions).
 */

#include "warftpd/warftpd.h"
#include "warftpd/Database.h"
#include "log/WarLog.h"
#include "war_uuid.h"
#include <tasks/WarThreadpool.h>

using namespace std;
using namespace war;
using namespace war::wfde;

namespace warftpd {
namespace impl {
    
/*! The default auth manager in warftpd, using the database for user accounts.
 */
class AuthManagerImpl : public AuthManager
{
public:
    
    // recommended in Meyers, Effective STL when internationalization and embedded
    // NULLs aren't an issue.  Much faster than the STL or Boost lex versions.
    struct ciLessLibC : public std::binary_function<std::string, std::string, bool> {
        bool operator()(const std::string &lhs, const std::string &rhs) const {
            return strcasecmp(lhs.c_str(), rhs.c_str()) < 0 ;
        }
    };
    
    
    class ClientImpl : public Client
    {
    public:
        using on_zero_callback_t = function<void ()>;
        
        ClientImpl(AuthManagerImpl& mgr, 
                   Database::UserData&& data, 
                   const on_zero_callback_t& onZeroCallback) 
        : auth_manager_{mgr}, user_data_(move(data)), on_zero_callback_{onZeroCallback}
        {
            LOG_TRACE2_FN << "User " << log::Esc(user_data_.login_name) << " "
            << user_data_.id << " is instantiated";
        }
        
        ~ClientImpl() {
            LOG_TRACE2_FN << "User " << log::Esc(user_data_.login_name) << ' '
                << boost::uuids::to_string(user_data_.id) << " is deleted (from memory).";
        }
        
        const string& GetLoginName() const override { return user_data_.login_name; }
        const boost::uuids::uuid& GetUuid() const override { return user_data_.id; }
        int GetNumInstances() const override { return instance_count_; }
        const Database::UserData& GetData() const noexcept { return user_data_; }
        
        Permissions::ptr_t GetPermissions() const override {
            //TODO: Implement refresh after n seconds
            
            unique_lock<mutex> lock(auth_lock_);
            if (perms_)
                return perms_;
            
            // Lazy loading of permissions from the database.
            auto defs = auth_manager_.GetDb().GetPermissions(*this);
            const_cast<Permissions::ptr_t&>(perms_) = CreatePermissions(defs);
            return perms_;
        }
        void IncInstanceCounter() { ++instance_count_; }
        void DecInstanceCounter() { 
            if (--instance_count_ == 0) {
                if (on_zero_callback_) {
                    on_zero_callback_();
                }
            }
        }
        
    private:
        AuthManagerImpl& auth_manager_;
        const Database::UserData user_data_;
        Permissions::ptr_t perms_;
        atomic_int instance_count_;
        on_zero_callback_t on_zero_callback_;
        mutable mutex auth_lock_;
    };
    
    // Wrapper needed to maintain the instance count for the client
    class ClientProxy : public Client
    {
    public:
        
        const std::string& GetLoginName() const override {
            return client_->GetLoginName();
        }
        
        const boost::uuids::uuid& GetUuid() const override {
            return client_->GetUuid();
        }
        
        int GetNumInstances() const override {
            return client_->GetNumInstances();
        }
        
        const ClientImpl& GetImpl() { return *client_; }
        
        ClientProxy(std::shared_ptr<ClientImpl>&& client)
        : client_{move(client)}
        {
            client_->IncInstanceCounter();
        }
        
        ~ClientProxy() {
            client_->DecInstanceCounter();
        }
        
        Permissions::ptr_t GetPermissions() const override {
            return client_->GetPermissions();
        }
        
    private:
        const std::shared_ptr<ClientImpl> client_;
    };
    
    AuthManagerImpl(Database::ptr_t db)
    : db_{db}
    {
    }
    
    Client::ptr_t Login(const string& name, const string& pwd) override {
        // Get the user data from the database
        WAR_ASSERT(host_ && "Must be initialized");
        auto client = GetClient(name);
        const auto& user = client->GetImpl().GetData();
        
        if (user.auth_type == AuthTypes::ANONYMOUS_USER) {
            return Authenticated(move(client));
        }
        
        if (pwd.empty()) {
            WAR_THROW_T(ExceptionNeedPasswd, "Need password");
        }
        
        // check the credentials, using the appropriate authentication type.
        switch(user.auth_type) {
            case AuthTypes::CLEAR_PASSWD:
                if (pwd.compare(user.passwd) == 0) {
                    return Authenticated(move(client));
                }
                WAR_THROW_T(ExceptionBadCredentials, "Password does not match");
            default:
                WAR_ASSERT_OR_THROW_T(false, ExceptionBadState, "Unsupported authentication type");
        }
    }
    
    void Join(Host::ptr_t host) override {
        host_ = move(host);
    }
    
    Database& GetDb() { return *db_; }
    
private:
    Client::ptr_t Authenticated(Client::ptr_t&& client) {
        // TODO: Add login-count
        // TODO: Check for too many logins fort this account.
        return move(client);
    }
    
    shared_ptr<ClientProxy> GetClient(const std::string& loginName) {
        shared_ptr<ClientImpl> client;
        
        {
            lock_guard<std::mutex> lock(mutex_);
            auto it = clients_.find(loginName);
            if (it != clients_.end()) {
                client = it->second;
                return make_shared<ClientProxy>(move(client));
            }
        }
        
        // Load the client from the database
        auto user_data = db_->FindUser(host_->GetId(), loginName);
        client = make_shared<ClientImpl>(*this, move(user_data), [this, client]() {
                LOG_TRACE3_FN << "Scheduling client " << client->GetUuid()
                    << " for removal from the client-cache.";
                    
                host_->GetServer()->GetIoThreadpool().PostWithTimer({
                    bind(&AuthManagerImpl::DeleteClientFromCache, this, client),
                    "DeleteClientFromCache"}, 3000);
            });
        
        lock_guard<std::mutex> lock(mutex_);
        clients_[loginName] = client;
        return make_shared<ClientProxy>(move(client));
    }
    
    void DeleteClientFromCache(std::weak_ptr<Client> client) {
        auto client_ref = client.lock();
        if (client_ref && client_ref->GetNumInstances() == 0) {
            LOG_TRACE3_FN << "Removing client " << client_ref->GetUuid()
                << " from the client-cache.";
            lock_guard<std::mutex> lock(mutex_);
            clients_.erase(client_ref->GetLoginName());
        }
    }
    
    Database::ptr_t db_;
    Host::ptr_t host_;
    std::map<std::string, std::shared_ptr<ClientImpl>, ciLessLibC> clients_;
    mutex mutex_;
};
    
} // impl

war::wfde::AuthManager::ptr_t CreateAuthManager(Database::ptr_t& db) {
    return make_shared<impl::AuthManagerImpl>(db);
}

} // warftpd

