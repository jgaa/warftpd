#include <warftpd/warftpd.h>
#include <warftpd/Database.h>
#include <warlib/WarLog.h>

using namespace std;
using namespace war;

namespace warftpd {
namespace impl {

Database::ptr_t CreateSqliteDbInstance(const war::wfde::Configuration& conf);

} // impl

Database::ptr_t Database::CreateInstance(const war::wfde::Configuration& conf) {
    auto db_type = conf.GetValue("/Database/Type", "sqlite");

    if (db_type.compare("sqlite") == 0) {
        return impl::CreateSqliteDbInstance(conf);
    }

    LOG_ERROR << "Unknown database type: " << log::Esc(db_type);
    WAR_THROW_T(ExceptionNotFound, db_type);
}

} // warftpd
