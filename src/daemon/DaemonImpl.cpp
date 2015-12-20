
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
    }
    
    ~DaemonImpl() {
    }
    
    void Start() override {
        
    }
    
    void Shutdown() override {
    }
    
    void WaitForServiceShutdown() override {
    }
    
private:
    Configuration::ptr_t conf_;
};
    
} // impl

Daemon::ptr_t Daemon::Create(Configuration::ptr_t& conf) {
    return make_shared<impl::DaemonImpl>(conf);
}

} // warftpd

