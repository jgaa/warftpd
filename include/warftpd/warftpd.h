#pragma once

/* This project is free software, released under the Gnu General Public License v3
 * See the file LICENSE for details.
 * 
 * This file contains the headers required for the library and consumers of the
 * warftpd library.
 */

#include "warftpd_config.h"
#include "wfde/wfde.h"

#ifndef WARFTPD_PROGRAM_NAME
#   define WARFTPD_PROGRAM_NAME "warftpd"
#endif

#ifndef WARFTPD_CONFIG_FILE_NAME 
#   define WARFTPD_CONFIG_FILE_NAME "warftpd.conf"
#endif

namespace warftpd {
    
class Database;

/*! War FTP Daemon version */
enum class Version
{
    MAJOR = 4,
    MINOR = 0
};

enum AuthTypes {
    ANONYMOUS_USER = 0,
    CLEAR_PASSWD = 1
};

war::wfde::AuthManager::ptr_t CreateAuthManager(std::shared_ptr<Database>& db);

} // namespace

std::ostream& operator << (std::ostream& o, const warftpd::Version& ver);

