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
        mSocks = std::make_shared<queue<socket>>();
		mConnected = false;

		auto logentry = logs::debug << LOC() << "Using host " << mHost << " with port " << mPort;
		logentry << " on " << (V4_ONLY ? "IPv4" : "IPv6") << logs::done;
	  
		memset(&hints, 0, sizeof hints);
		hints.ai_family = V4_ONLY ? AF_INET : AF_UNSPEC; // IPv4 or 6 (AF_INET or AF_INET6)
		hints.ai_socktype = SOCK_STREAM; // TCP
		hints.ai_flags = AI_PASSIVE; // Autodetect local host

		logs::debug << LOC() << "Resolving host..." << logs::done;
		if (int ret = getaddrinfo(mHost, std::to_string(mPort).c_str(), &hints, &mAddrInfoPtr))
		{
			auto logentry = logs::error << LOC() << "Failed to resolve host: " << mHost;
			logentry << " (" << ret << ":" << gai_strerror(ret) << ")" << logs::done;
			throw std::runtime_error(LOC() "Hostname not resolved");
		}
		logs::debug << LOC() << "Done." << logs::done;

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

			logs::debug << LOC() << "Connecting to " << mHost << " (" << ip << ")... " << logs::done;

			if ( (mSockfd = ::socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) < 0 )
			{ 
				logs::warn << LOC() << "Could not create socket" << logs::done;
				continue;
			}

			int ret;
			for (int i = 1; i < (1 << MAX_RETRY); i *= 2)
			{
				if ((ret = connect(mSockfd, ai->ai_addr, ai->ai_addrlen)))
				{
					logs::warn << LOC() << "(" << mSockfd << ") Connection to " << ip << " failed [" << std::strerror(errno) << "]" << logs::done;
					logs::debug << LOC() << "(" << mSockfd << ") Retrying in " << i << "..." << logs::done;
					std::this_thread::sleep_for(std::chrono::milliseconds(1000*i));
				}
				else break;
			}
			if (!ret) break;
		}
		if ( ai == NULL ) 
		{
			logs::error << LOC() << "(" << mSockfd << ") Could not connect to host: " << mHost << logs::done;
			throw std::runtime_error(LOC() "Connection failed");
		}
		else
		{
			logs::info << LOC() << "(" << mSockfd << ") Connected to " << mHost << "!" << logs::done;
			mConnected = true;
		}
	}

	socket::socket(unsigned short pPort)
		: mPort(pPort)
	{
		struct sockaddr_in serv_addr;
        int option = 1;

		mConnected = false;

        mSocks = std::make_shared<queue<socket>>();

		mSockfd = ::socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(mSockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
		if (mSockfd < 0) 
		{
			logs::error << LOC() << "(" << mSockfd << ") Failed opening socket" << logs::done;
			return;
		}

		memset((char *) &serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(mPort);

		int ret;
		for (int i = 1; i < (1 << MAX_RETRY); i *= 2)
		{
			if ((ret = bind(mSockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0))
			{
				logs::error << LOC() << "(" << mSockfd << ") Failed to bind socket [" << std::strerror(errno) << "]" << logs::done;
				logs::debug << LOC() << "(" << mSockfd << ") Retrying in " << i << "..." << logs::done;
				std::this_thread::sleep_for(std::chrono::milliseconds(1000*i));
			}
			else break;
		}
		if (ret) return;

		::listen(mSockfd, 5);
	}

	socket::socket(int pSockfd)
		: mSockfd(pSockfd)
	{	
        mSocks = std::make_shared<queue<socket>>();
        mConnected = true;
	}

	void socket::listen()
	{
		logs::debug << LOC() << "Socket listening" << logs::done;
		int sockfd;
		socklen_t clilen;
		struct sockaddr_in cli_addr;
		char buffer[512] = {0};

		clilen = sizeof(cli_addr);
		sockfd = accept(mSockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (sockfd < 0) 
		{
			logs::error << LOC() << "(" << sockfd << ") Failed to accept connection on socket" << logs::done;
			return;
		}
		else
		{
			inet_ntop(AF_INET, &cli_addr, buffer, clilen);
			logs::info << LOC() << "(" << sockfd << ") Accepted connection: " << buffer << logs::done;
			socket sock(sockfd);
			mSocks->push(sock);
			listen();
		}
	}

	void socket::send(const char *pFmt)
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);
		mOutBuf << pFmt;
        if (thread::get_current_name() != "main")
        {
            std::string logLine = mOutBuf.str();
            logs::info << "-<- " << logLine << logs::done;
        }
		mOutBuf << "\r\n";
		std::string line = mOutBuf.str();
		ssize_t len = line.size();

		if (!mConnected) return;

		while (ssize_t diff = len - ::send(mSockfd, line.c_str(), len, false))
		{
			if (len - diff < 0)
			{
                if (thread::get_current_name() != "main")
                {
                    logs::warn << "Disconnected" << logs::done;
                }
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
		std::vector<std::string> lines;

        logs::debug << LOC() << "(" << mSockfd << ") Starting Recv" << logs::done;
		char data[IRC_MAXBUF] = {0};
		ssize_t carry = strlen(mInBuf);
		strcpy(data, mInBuf);
		memset(mInBuf, 0, IRC_MAXLINE);
		ssize_t len = ::recv(mSockfd, data+carry, IRC_MAXBUF-carry-1, false);
        logs::debug << LOC() << "(" << mSockfd << ") Recv got " << len << " bytes" << logs::done;
		if (len <= 0)
		{
			logs::warn << LOC() << "(" << mSockfd << ") Disconnected" << logs::done;
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
				logs::info << "->- " << line << logs::done;
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

	bool socket::connected() const
	{
		return mConnected;
	}

	void socket::close()
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);
		::shutdown(mSockfd, 2);
		mConnected = false;
	}

	socket socket::get()
	{
		return mSocks->pop();
	}

    bool socket::operator==(const socket& pSock) const
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
			mOutBuf.str(pSock.mOutBuf.str());
            mSocks = pSock.mSocks;
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
		mOutBuf.str(pSock.mOutBuf.str());
        mSocks = pSock.mSocks;
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
