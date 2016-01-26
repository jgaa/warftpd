
/* TODO:
 *   - Implement load of all objects
 *   - Implement shutdown
 */

#include <vector>
#include <boost/lexical_cast.hpp>
#include <log/WarLog.h>
#include "war_error_handling.h"
#include <tasks/WarThreadpool.h>
#include "warftpd/warftpd.h"
#include "warftpd/Database.h"
#include "Daemon.h"

using namespace std;
using namespace war;
using namespace war::wfde;

// TODO: Move to library
std::ostream& operator << (std::ostream& o, const warftpd::Version& ver)
{
    return o
        << static_cast<int>(warftpd::Version::MAJOR)
        << '.'
        << static_cast<int>(warftpd::Version::MINOR);
}


namespace warftpd {
       
namespace impl {

class DaemonImpl : public Daemon
{
public:
    DaemonImpl(Configuration::ptr_t conf)
    : conf_{conf}
    {
        auto& logger = log::LogEngine::GetInstance();
        // Set up logging
        if (!boost::lexical_cast<bool>(conf->GetValue("/System/Daemon", "0"))
            && conf->HaveValue("/Log/ConsoleLevel")) {
            string level = conf->GetValue("/Log/ConsoleLevel");
            logger.AddHandler(make_shared<log::LogToStream>(cout, "console",
                log::LogEngine::GetLevelFromName(level)));
        }
        
        if (conf->HaveValue("/Log/File")) {
            string path = conf->GetValue("/Log/File");
            string level = conf->GetValue("/Log/Level", "NOTICE");
            bool truncate = boost::lexical_cast<bool>(conf->GetValue("/Log/Truncate"));
            logger.AddHandler(make_shared<log::LogToFile>(
                path,
                truncate,
                "file",
                log::LogEngine::GetLevelFromName(level)));
        }
        
        // TODO: Add syslog/eventlog handler
        
        RegisterDefaultProtocols();   
    }
    
    ~DaemonImpl() {
    }
    
    Permissions::ptr_t GetPermissions(const Client& user) override {
        return nullptr;
    }
    
    Permissions::ptr_t GetPermissions(const Entity& entity) override {
        return nullptr;
    }
    
    void Start() override {
        thread_pool_ = std::make_unique<Threadpool>(
            boost::lexical_cast<int>(conf_->GetValue("/System/NumIoThreads", "0")),
            boost::lexical_cast<int>(conf_->GetValue("/System/MaxIoThreadQueueCapacity", "1024")));
        db_ = Database::CreateInstance(*conf_);
        if (boost::lexical_cast<bool>(conf_->GetValue("/Database/Bootstrap", "0"))) {
            LOG_NOTICE_FN << "Bootstrapping the database. Any existing data will be deleted.";
            db_->Bootstrap();
        }
        
        LoadAllEntities();
    }
    
    void Shutdown() override {
        thread_pool_->Close();
    }
    
    void WaitForServiceShutdown() override {
        thread_pool_->WaitUntilClosed();
    }

private:
    void SetPermissions(Entity& entity) {
        auto defs = db_->GetPermissions(entity);
        if (defs) {
            auto perms = CreatePermissions(defs);
            entity.SetPermissions(perms);
        }
    }
    
    void LoadAllEntities() {
	LOG_DEBUG_FN << "Begin loading entities:";
        for(auto& sc : db_->FindServer().conf) {
            if (!boost::lexical_cast<bool>(sc->GetValue("/Enabled", "1")))
                continue;
            
            // Instantiate the server
	    LOG_DEBUG_FN << "  -->> Server: " << sc->GetValue("/Name");
            auto server = CreateServer(sc, *thread_pool_);
            SetPermissions(*server);
            
            // Load hosts
            for(auto& hc : db_->FindHost(*server).conf) {
              
	      LOG_DEBUG_FN << "  ---->> Host: " << hc->GetValue("/Name");
	      
	      auto am = CreateAuthManager(db_);
                auto host = CreateHost(*server, am, hc);
                SetPermissions(*host);
                
                // TODO: Add permissions
                
                for(auto& pc : db_->FindProtocol(*host).conf) {
                    
		    LOG_DEBUG_FN << "  ------>> Protocol: " << pc->GetValue("/Name");
                    auto prot = CreateProtocol(host.get(), pc);
                    SetPermissions(*prot);
                    
                    for(auto& inf : db_->FindInterface(*prot).conf) {
			LOG_DEBUG_FN << "  -------->> Interface: " << inf->GetValue("/Name");
                        prot->AddInterface(inf);
                    }
                }
            }
            
            servers_.push_back(move(server));
        }
    }
    
    Database::ptr_t db_;
    std::unique_ptr<Threadpool> thread_pool_;
    Configuration::ptr_t conf_;
    vector<Server::ptr_t> servers_;
};
    
} // impl

Daemon::ptr_t Daemon::Create(Configuration::ptr_t& conf) {
    return make_shared<impl::DaemonImpl>(conf);
}

} // warftpd

