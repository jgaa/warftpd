# War FTP Daemon
--------------------------------------

This is a new implementation of the War FTP Daemon.

War FTP Daemon consists of three libraries written by Jarle (jgaa) Aase.

    warftpd (GPLv3)
        [depends on] wfde (GPLv3)
            [depends on] warlib (LGPLv3)


Warlib is a rather general C++ library that implements trivial, reusable things like
logging, a thread-pool and some helper classes around some common boost libraries.
Warlib depends on boost::asio for IO and threads. It uses the C++14 library whenever
possible, and falls back to boost when needed.

Wfde is a complete implementation of the relevant parts of the FTP protocol.
It also implements HTTP so that a file sharing site can use one common configuration
for both FTP and HTTP. The HTTP classes is used by warftpd also for the user
interface, and it may be used in the future to provide a REST interface to the server.
The implementation follow best practice and use a fixed number of IO threads no
matter how many user sessions or file transfers that are active. All network
IO operations are asynchronous, and all network IO operations for a certain connection
are scheduled on the same CPU/Core to prevent cache congestion while reading/writing
to the same IO buffers from multiple CPU Cores.

The warftpd library implements all that is unique for the War FTP Daemon. That means
the actual user/object model, the User Interface infrastructure, and additional
features that have made the War FTP Daemon popular for 20 years.

Other dependencies:
  - OpenSSL (or a plug-in compatible library)
  - boost

*Status*: Under development
