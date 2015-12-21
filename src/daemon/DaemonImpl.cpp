
#include <boost/lexical_cast.hpp>
#include <log/WarLog.h>
#include "war_error_handling.h"
#include <tasks/WarThreadpool.h>
#include "warftpd/warftpd.h"
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
        
    }
    
    ~DaemonImpl() {
    }
    
    void Start() override {
        thread_pool_ = std::make_unique<Threadpool>(
            boost::lexical_cast<int>(conf_->GetValue("/System/NumIoThreads", "0")),
            boost::lexical_cast<int>(conf_->GetValue("/System/MaxIoThreadQueueCapacity", "1024")));
    }
    
    void Shutdown() override {
        thread_pool_->Close();
    }
    
    void WaitForServiceShutdown() override {
        thread_pool_->WaitUntilClosed();
    }

private:
    Configuration::ptr_t conf_;
    std::unique_ptr<Threadpool> thread_pool_;
};
    
} // impl

Daemon::ptr_t Daemon::Create(Configuration::ptr_t& conf) {
    return make_shared<impl::DaemonImpl>(conf);
}

} // warftpd

