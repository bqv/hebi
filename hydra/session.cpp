#include "session.hpp"

namespace hydra
{
    session::session(unsigned short pPort)
        : mSock(pPort), nodeID(rand())
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

    void session::broadcast(server& pSrv, message pMsg)
    {
        if (std::find(mSeen.begin(), mSeen.end(), pMsg) != mSeen.end())
        {
            return;
        }
        mSeen.push_back(pMsg);
        if (mSeen.size() > 512)
        {
            mSeen.pop_front();
        }

        for (server srv : mServers)
        {
            if (srv == pSrv)
            {
                continue;
            }
            srv.send(pMsg);
        }
        for (client clnt : mClients)
        {
            clnt.send(pMsg);
        }
    }

    void session::broadcast(client& pClnt, message pMsg)
    {
        if (std::find(mSeen.begin(), mSeen.end(), pMsg) != mSeen.end())
        {
            return;
        }
        mSeen.push_back(pMsg);
        if (mSeen.size() > 512)
        {
            mSeen.pop_front();
        }

        for (server srv : mServers)
        {
            srv.send(pMsg);
        }
        for (client clnt : mClients)
        {
            if (clnt == pClnt)
            {
                continue;
            }
            clnt.send(pMsg);
        }
    }

    session::~session()
    {
    }
}
