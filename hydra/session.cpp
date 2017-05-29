#include "session.hpp"

namespace hydra
{
    session::session(unsigned short pPort)
        : mSock(pPort), mNodeId(rand())
    {
		mListener = thread::make_thread("hydra::session::listen", &session::listen, this);
		mListener.detach();
    }

    void session::connect(const std::string pHost, unsigned short pPort)
    {
        mSock = sockets::socket(pHost.c_str(), pPort);
		mClients.emplace_back(mSock, this);
    }

    void session::listen()
    {
        std::thread listener = thread::make_thread("sockets::socket::listen", &sockets::socket::listen, mSock);
		listener.detach();

		for(;;)
		{
            logs::debug << LOC() << "Waiting for socket" << logs::done;
			sockets::socket sock = mSock.get();
            logs::debug << LOC() << "Creating server..." << logs::done;
			mServers.emplace_back(sock, this);
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

	std::uint32_t session::nodeId()
	{
		return mNodeId;
	}

    session::~session()
    {
    }
}
