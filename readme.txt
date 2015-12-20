War FTP Daemon 2016
--------------------------------------

War FTP Daemon consists of three libraries written by Jarle (jgaa) ASase.

    warftpd (GPLv3)
        [depends on] wfde (GPLv3)
            [depends on] warlib (LGPLv3)
            

Warlib is a rather general C++11 libary that implements trivial, reusable things like
logging, a threadpool and some helper classes around some common boost libraries.
Warlib depends on boost::asio for IO and threads. It uses the C++14 library whenever
possible, and falls back to boost when needed.

Wfde is a complete implementation of the FTP protocol. It also implements HTTP so that
a file sharing site can use one common configuration for both FTP and HTTP. The HTTP
classes is used by warftpd also for the user interface, and it may be used in the
future to provide a REST interface to the server. The implementation follow best
practic and use a fixed number of IO threads no matter how many user sessions or
file transfers that are active. All network IO operations are asyncronous, and 
all network IO opertations for a certain connection is scheduled on the same
CPU/Core to prevent cache congestion while reading/writing to the same IO buffers
from multiple CPU Cores. 

The Warftpd library implements all that is unique for the War FTP Daemon. That means 
the actual user/object model, the User Interface infrastructure, and additional 
features that have made the War FTP Daemon popular for 20 years.

Other dependencies:
  - OpenSSL (or a plug-in compatible library)
  - boost
  - zlib

Reqired tools:
  - g++ version 9 or better or clang++ version 3.5 or better
  - cmake
  - doxygen


How to get the code and compile it for Linux:

   $ git clone git://git.code.sf.net/p/wfde/warftpd/git warftpd
   $ cd warftpd
   $ git submodule update --init
   $ mkdir dbuild
   $ cd dbuild
   $ cmake -DDEBUG -G "Unix Makefiles" ../
   $ make
   
