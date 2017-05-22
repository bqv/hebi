#include "socket.hpp"

namespace sockets
{
	socket::socket(const char* pHost, unsigned short pPort)
		: mHost(pHost)
		, mPort(pPort)
	{
		struct addrinfo hints, *ai;
		char ip[INET6_ADDRSTRLEN];

		std::fill(mInBuf, mInBuf+sizeof(mInBuf), 0);
		mConnected = false;

		auto logentry = log::debug << LOC() << "Using host " << mHost << " with port " << mPort;
		logentry << " on " << (V4_ONLY ? "IPv4" : "IPv6") << log::done;
	  
		memset(&hints, 0, sizeof hints);
		hints.ai_family = V4_ONLY ? AF_INET : AF_UNSPEC; // IPv4 or 6 (AF_INET or AF_INET6)
		hints.ai_socktype = SOCK_STREAM; // TCP
		hints.ai_flags = AI_PASSIVE; // Autodetect local host

		log::debug << LOC() << "Resolving host..." << log::done;
		if (int ret = getaddrinfo(mHost, std::to_string(mPort).c_str(), &hints, &mAddrInfoPtr))
		{
			auto logentry = log::error << LOC() << "Failed to resolve host: " << mHost;
			logentry << " (" << ret << ":" << gai_strerror(ret) << ")" << log::done;
			throw std::runtime_error(LOC() "Hostname not resolved");
		}
		log::debug << LOC() << "Done." << log::done;

		for ( ai = mAddrInfoPtr; ai != NULL; ai = ai->ai_next )
		{
			void *server; // sockaddr_in or sockaddr_in6
			if ( ai->ai_family == AF_INET )
			{ // IPv4
				struct sockaddr_in *ipv4 = (struct sockaddr_in *)ai->ai_addr;
				server = &(ipv4->sin_addr);
			}
			else
			{ // IPv6
				struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ai->ai_addr;
				server = &(ipv6->sin6_addr);
			}
			inet_ntop(ai->ai_family, server, ip, sizeof ip);

			log::debug << LOC() << "Connecting to " << mHost << " (" << ip << ")... " << log::done;

			if ( (mSockfd = ::socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0 )
			{ 
				log::warn << LOC() << "Could not create socket" << log::done;
				continue;
			}

			if (int ret = connect(mSockfd, ai->ai_addr, ai->ai_addrlen))
			{
				log::warn << LOC() << "Connection to " << ip << " failed [" << ret << "]" << log::done;
			}
			else break;
		}
		if ( ai == NULL ) 
		{
			log::error << LOC() << "Could not connect to host: " << mHost << log::done;
			throw std::runtime_error(LOC() "Connection failed");
		}
		else
		{
			log::info << "Connected to " << mHost << "!" << log::done;
			mConnected = true;
		}
	}

	socket::socket(unsigned short pPort)
		: mPort(pPort)
	{
		struct sockaddr_in serv_addr;

		mSockfd = ::socket(AF_INET, SOCK_STREAM, 0);
		if (mSockfd < 0) 
		{
			log::error << "Failed opening socket" << log::done;
			return;
		}

		memset((char *) &serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(mPort);

		if (bind(mSockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		{
			log::error << "Failed to bind socket" << log::done;
			return;
		}

		::listen(mSockfd, 5);
	}

	socket::socket(int pSockfd)
		: mSockfd(pSockfd)
	{
	}

	void socket::listen()
	{
		int sockfd;
		socklen_t clilen;
		struct sockaddr_in cli_addr;
		char buffer[512] = {0};

		clilen = sizeof(cli_addr);
		sockfd = accept(mSockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (sockfd < 0) 
		{
			log::error << "Failed to accept connection on socket" << log::done;
			return;
		}
		else
		{
			inet_ntop(AF_INET, &cli_addr, buffer, clilen);
			log::info << "Accepted connection: " << buffer << log::done;
			class socket sock(sockfd);
			mSocks.push(sock);
			listen();
		}
	}

	void socket::send(const char *pFmt)
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);
		mOutBuf << pFmt;
		log::info << "-<- " << mOutBuf.str() << log::done;
		mOutBuf << "\r\n";
		std::string line = mOutBuf.str();
		ssize_t len = line.size();

		if (!mConnected) return;

		while (ssize_t diff = len - ::send(mSockfd, line.c_str(), len, false))
		{
			if (len - diff < 0)
			{
				log::warn << "IRC Disconnected" << log::done;
				mConnected = false;
				return;
			}

			line = line.substr(diff, std::string::npos);
			len = line.size();
		}
		mOutBuf.clear();
		mOutBuf.str("");
	}

	std::vector<std::string> socket::recv()
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);
		std::vector<std::string> lines;

		char data[IRC_MAXBUF] = {0};
		ssize_t carry = strlen(mInBuf);
		strcpy(data, mInBuf);
		memset(mInBuf, 0, IRC_MAXLINE);
		ssize_t len = ::recv(mSockfd, data+carry, IRC_MAXBUF-carry-1, false);
		if (len <= 0)
		{
			log::warn << "IRC Disconnected" << log::done;
			mConnected = false;
			return lines;
		}
		data[carry+len] = '\0';
		
		for (const char *start = data; start < data+IRC_MAXBUF-1;)
		{
			const char *end = strchr(start, '\r');
			if (end && end[1] == '\n')
			{
				std::string line(start, end-start);
				log::info << "->- " << line << log::done;
				lines.push_back(line);
				start = end+2;
			}
			else
			{
				strncpy(mInBuf, start, IRC_MAXLINE-1);
				break;
			}
		}
		return lines;
	}

	bool socket::connected()
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);
		return mConnected;
	}

	void socket::close()
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);
		::shutdown(mSockfd, 2);
		mConnected = false;
	}

	class socket socket::get()
	{
		return mSocks.pop();
	}

    bool socket::operator==(const socket& pSock)
    {
        return mSockfd == pSock.mSockfd;
    }

	socket& socket::operator=(const socket& pSock)
	{
		if (this != &pSock)
		{
			mHost = pSock.mHost;
			mPort = pSock.mPort;
			mSockfd = pSock.mSockfd;
			std::fill(mInBuf, mInBuf+sizeof(mInBuf), 0);
            mAddrInfoPtr = NULL;
			mConnected = true;
		}
		return *this;
	}

	socket::socket(const socket& pSock)
	{
		mHost = pSock.mHost;
		mPort = pSock.mPort;
		mSockfd = pSock.mSockfd;
		std::fill(mInBuf, mInBuf+sizeof(mInBuf), 0);
        mAddrInfoPtr = NULL;
		mConnected = true;
	}

	socket::~socket()
	{
		if (mAddrInfoPtr)
        {
            freeaddrinfo(mAddrInfoPtr);
        }
	}
}
