#include "socket.hpp"

namespace irc
{
    socket::socket(const char* pHost, unsigned short pPort)
        : mHost(pHost)
        , mPort(pPort)
    {
        assert(pPort > 0);

        struct addrinfo hints, *ai;
        char ip[INET6_ADDRSTRLEN];

        std::fill(mInBuf, mInBuf+sizeof(mInBuf), 0);

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
            log::info << "Connected to IRC!" << log::done;
        }
    }

    void socket::send(const char *pFmt)
    {
        std::lock_guard<std::recursive_mutex> guard(mLock);
        mOutBuf << pFmt;
        log::info << "-<- " << mOutBuf.str() << log::done;
        mOutBuf << "\r\n";
        std::string line = mOutBuf.str();
        size_t len = line.size();

        while (size_t diff = len - ::send(mSockfd, line.c_str(), len, false))
        {
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
        size_t carry = strlen(mInBuf);
        strcpy(data, mInBuf);
        size_t len = ::recv(mSockfd, data+carry, IRC_MAXBUF-carry-1, false);
        data[len] = '\0';
        
        for (const char *start = data;;)
        {
            if (const char *end = strchr(start, '\n'))
            {
                std::string line(start, end-start-1);
                log::info << "->- " << line << log::done;
                lines.push_back(line);
                start = end+1;
            }
            else
            {
                strncpy(mInBuf, start, IRC_MAXLINE-1);
                break;
            }
        }
        return lines;
    }

    socket::~socket()
    {
        freeaddrinfo(mAddrInfoPtr);
    }
}
