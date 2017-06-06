#ifndef _IRC_SOCKET_HPP_
#define _IRC_SOCKET_HPP_

#include <cassert>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
#include <cerrno>
#include <cstring>
#include <thread>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include "config.hpp"
#include "logger.hpp"
#include "queue.hpp"

static_assert(IRC_MAXBUF >= IRC_MAXLINE, "IRC_MAXBUF must be greater that or equal to IRC_MAXLINE");

namespace sockets
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
        std::shared_ptr<queue<socket>> mSocks;
		bool mConnected;

	  public:
		socket(const char* pHost, unsigned short pPort);
		socket(unsigned short pPort);
		socket(int pSockfd);
		socket(const socket& pSock);
		socket& operator=(const socket& pSock);
		~socket();

        bool operator==(const socket& pSock) const;
		void listen();
		template<typename T, typename... Types>
		void send(const char *pFmt, T pValue, Types... pRest);

		void send(const char *pFmt);
		std::vector<std::string> recv();
		bool connected() const;
		void close();
		class socket get();
	};
}

#include "socket.ipp"

#endif /*IRC_SOCKET_HPP*/
