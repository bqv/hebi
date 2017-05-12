#ifndef _IRC_SOCKET_HPP_
#define _IRC_SOCKET_HPP_

#include <cassert>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>

#include "../config.hpp"
#include "../logger.hpp"

static_assert(IRC_MAXBUF >= IRC_MAXLINE, "IRC_MAXBUF must be greater that or equal to IRC_MAXLINE");

namespace irc
{
    class socket
    {
      private:
        const char* mHost;
        unsigned short mPort;
        int mSockfd;
        struct addrinfo *mAddrInfoPtr;
        std::ostringstream mOutBuf;
        char mInBuf[IRC_MAXLINE];
        std::recursive_mutex mLock;
        bool mConnected;

      public:
        socket(const char* pHost, unsigned short pPort);
        ~socket();

        template<typename T, typename... Types>
        void send(const char *pFmt, T pValue, Types... pRest);
        void send(const char *pFmt);
        std::vector<std::string> recv();
        bool connected();
    };
}

#include "socket.ipp"

#endif /*IRC_SOCKET_HPP*/
