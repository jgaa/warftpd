/* This project is free software, released under the Gnu General Public License v3
 * See the file LICENSE for details.
 *
 * This file contains the main function for the War FTP Daemon.
 *
 */

#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <warlib/WarLog.h>
#include <warlib/error_handling.h>
#ifdef WIN32
#   include <warlib/win/minidump.h>
#endif
#include <warftpd/warftpd.h>
#include "Daemon.h"

using namespace std;
using namespace war;
using namespace wfde;

struct CmdLineOptions
{
    boost::optional<int> num_io_threads;
    boost::optional<int> max_io_thread_queue_capacity;
#ifndef WIN32
    boost::optional<bool> daemon;
#endif
    string conf_file {WARFTPD_CONFIG_FILE_NAME};

    boost::optional<bool> truncate_log;;
    boost::optional<string> console_log_level;
    boost::optional<string> log_level;
    boost::optional<string> log_file;
    boost::optional<bool> bootstrap;
};

void SleepUntilDoomdsay()
{
    io_service_t main_thread_service;

    boost::asio::signal_set signals(main_thread_service, SIGINT, SIGTERM
#ifdef SIGQUIT
        ,SIGQUIT
#endif
        );
    signals.async_wait([](boost::system::error_code /*ec*/, int signo) {

        LOG_INFO << "Reiceived signal " << signo << ". Shutting down";
    });

    LOG_DEBUG_FN << "Main thread going to sleep - waiting for shtudown signal";
    main_thread_service.run();
    LOG_DEBUG_FN << "Main thread is awake";
}

bool ParseCommandLine(int argc, char *argv[], log::LogEngine& logger,
                      CmdLineOptions& conf)
{
    namespace po = boost::program_options;

    po::options_description general("General Options");

    general.add_options()
        ("help,h", "Print help and exit")
        ("config-file,c",
            po::value<string>(&conf.conf_file)->default_value(conf.conf_file),
            "Configuration file")
        ("console-log,C", po::value<string>(),
            "Log-level for the console-log")
        ("log-level,L",
            po::value<string>(),
            "Log-level for the log-file")
        ("log-file",
            po::value<string>(),
            "Name of the log-file")
        ("truncate-log",
            po::value<bool>(),
            "Truncate the log-file if it already exists")
        ("bootstrap",
         "Bootstrap the database (erases all data)")
#ifndef WIN32
        ("daemon", po::value<string>(), "Run as a system daemon")
#endif
        ;

    po::options_description performance("Performance Options");
    performance.add_options()
        ("io-threads",
            "Number of IO threads. If 0, a reasonable value will be used")
        ("io-queue-size",
            "Capacity of the IO thread queues (max number of pending tasks per thread)")
        ;

    po::options_description cmdline_options;
    cmdline_options.add(general).add(performance);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << cmdline_options << endl
            << "Log-levels are:" << endl
            << "   FATAL ERROR WARNING INFO NOTICE DEBUG " << endl
            << "   TRACE1 TRACE2 TRACE3 TRACE4" << endl;

        return false;
    }

    if (
#ifndef WIN32
        !conf.daemon &&
#endif
        vm.count("console-log")
    ) {
        conf.console_log_level = vm["console-log"].as<string>();
    }
    if (vm.count("log-level")) {
        conf.log_level = vm["log-level"].as<string>();
    }
    if (vm.count("log-file")) {
        conf.log_file = vm["log-file"].as<string>();
    }
    if (vm.count("truncate-log")) {
        conf.truncate_log = vm["truncate-log"].as<bool>();
    }
    if (vm.count("io-threads"))
        conf.num_io_threads = vm["io-threads"].as<int>();
    if (vm.count("io-queue-size")) {
        conf.max_io_thread_queue_capacity = vm["io-queue-size"].as<int>();
    }
    if (vm.count("bootstrap")) {
        conf.bootstrap = true;
    }

    return true;
}


#define APPLY_IF(clo, node) {\
    if (clo) { \
        conf->SetValue(node, boost::any_cast<string>(*clo)); \
    }}

int main(int argc, char *argv[]) {
    log::LogEngine logger;
    bool command_mode = false;

#ifdef WIN32
    /*
     * Enable minidump generation if the application crash under Windows
     */
    EnableMinidump(WARFTPD_PROGRAM_NAME);
#endif

    try {

        Configuration::ptr_t conf;
        {
           /*
            * Create a configuration
            */
            CmdLineOptions options;

            if (!ParseCommandLine(argc, argv, logger, options))
                return -1;

            if (!boost::filesystem::exists(options.conf_file)) {
                LOG_WARN << "The configuration-file \""
                    << options.conf_file
                    << "\" was not found.";
                conf = war::wfde::Configuration::CreateInstance();
            } else {
                conf = war::wfde::Configuration::GetConfiguration(options.conf_file);
            }
            WAR_ASSERT(conf != nullptr);

            // Override the options with those from the command-line.
            APPLY_IF(options.truncate_log, "/Log/Truncate");
            APPLY_IF(options.console_log_level, "/Log/ConsoleLevel");
            APPLY_IF(options.log_level, "/Log/Level");
            APPLY_IF(options.log_file, "/Log/File");

            APPLY_IF(options.num_io_threads, "/System/NumIoThreads");
            APPLY_IF(options.max_io_thread_queue_capacity, "/System/MaxIoThreadQueueCapacity");
            APPLY_IF(options.daemon, "/System/Daemon");
            if (options.bootstrap && *options.bootstrap) {
                conf->SetValue("/Database/Bootstrap", "1");
                command_mode = true;
            }

    #ifndef WIN32
            if (boost::lexical_cast<bool>(conf->GetValue("/System/Daemon", "0"))) {
                LOG_INFO << "Switching to system daemon mode";
                daemon(1, 0);
            }
    #endif
        }

        auto daemon = warftpd::Daemon::Create(conf);

        LOG_INFO << WARFTPD_PROGRAM_NAME << ' ' << warftpd::Version() << " starting up...";

        daemon->Start();

        /* We now put the main-thread to sleep.
         *
         * It will remain sleeping until we receive one of the common
         * shutdown/quit siglals.
         */
        if (!command_mode) {
            SleepUntilDoomdsay();
        }

        daemon->Shutdown();
        daemon->WaitForServiceShutdown();

    } catch(const war::ExceptionBase& ex) {
        LOG_ERROR_FN << "Caught exception: " << ex;
        return -1;
    } catch(const boost::exception& ex) {
        LOG_ERROR_FN << "Caught boost exception: " << ex;
        return -1;
    } catch(const std::exception& ex) {
        LOG_ERROR_FN << "Caught standard exception: " << ex;
        return -1;
    } catch(...) {
        LOG_ERROR_FN << "*** Caught UNKNOWN exception!";
        return -1;
    };

    LOG_INFO << "So Long, and Thanks for All the Fish!";


    return 0; // Everything is fine
}
