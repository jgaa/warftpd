War FTP Daemon 2016
--------------------------------------

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
  - zlib

Required tools:
  - g++ version 5.9 or better or clang++ version 3.5 or better
  - cmake
  - doxygen



Obtaining and compiling the code:
-------------------------------------------------------------------

How to get the code and compile it for Linux:

   $ git clone git://git.code.sf.net/p/wfde/warftpd/git warftpd
   $ cd warftpd
   $ git submodule update --init
   $ mkdir dbuild
   $ cd dbuild
   $ cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ../
   $ make


Syncronizing the submodules with the latest upstream versions
-------------------------------------------------------------------
  $ git submodule foreach git checkout master
  $ git submodule foreach git pull origin master


Functional Testing:
-------------------------------------------------------------------

1) Testing the wfde library:

    In order to run the functional tests, you first run basic_tests.py, and then you start
    the wfde test FTP server with a configuration-file created by the test script.

    Example:
    [In a shell session in the warftpd dir]:
    ~/tmp/warftpd$ ./externals/wfde/tests/functional/basic_tests.py
    Creating path: /home/jgaa/src/wfde/test-tmp/ftproot
    Creating path: /home/jgaa/src/wfde/test-tmp/client
    Creating path: /home/jgaa/src/wfde/test-tmp/ftproot/home/jgaa
    Creating path: /home/jgaa/src/wfde/test-tmp/ftproot/upload
    Creating path: /home/jgaa/src/wfde/test-tmp/ftproot/pub/sub/sub2
    Creating path: /home/jgaa/src/wfde/test-tmp/ftproot/empty
    Creating missing test-files for download
    This may take a few minutes...
    Ready to start tests on ftp-root: /home/jgaa/src/wfde/test-tmp/ftproot
    Start the server with config-path to /home/jgaa/src/wfde/test-tmp/wfded.conf
    Press ENTER when ready
    ...

    [In another shell session]:
    ~/tmp/warftpd$ ./dbuild/externals/wfde/tests/wfded/wfdedD -c /home/jgaa/tmp/warftpd/test-tmp/wfded.conf
    ...

    Now, press ENTER in the first shell session and wait for the tests to finish.

    Press ^C in the second session to kill the FTP server when the tests have finished.

