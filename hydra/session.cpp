#include "session.hpp"

namespace hydra
{
    session::session(unsigned short pPort)
        : mSock(pPort)
    {
		mListener = std::thread(&session::listen, this);
		mListener.detach();
    }

    void session::connect(const std::string pHost, unsigned short pPort)
    {
        mSock = sockets::socket(pHost.c_str(), pPort);
		mClients.push_back(client(mSock, this));
    }

    void session::listen()
    {
        std::thread listener(&sockets::socket::listen, mSock);
		listener.detach();

		for(;;)
		{
			sockets::socket sock = mSock.get();
			server srv(sock, this);
			mServers.push_back(srv);
		}
    }

    session::~session()
    {
    }
}
