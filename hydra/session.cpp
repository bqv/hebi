#include "session.hpp"

namespace hydra
{
    session::session(unsigned short pPort)
        : mSock(pPort), mNodeId(rand()), mNodes({mNodeId})
    {
        mState = state::TERMTIME;
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
        logs::debug << LOC() "Broadcasting message: " << pMsg.serialize() << logs::done;
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
    
    void session::broadcast(message pMsg)
    {
        for (server srv : mServers)
        {
            srv.send(pMsg);
        }
        for (client clnt : mClients)
        {
            clnt.send(pMsg);
        }
    }

    void session::addNode(std::uint32_t pId)
    {
        mNodes.insert(pId);
    }

    void session::rmNode(std::uint32_t pId)
    {
        mNodes.erase(pId);
    }

    std::set<std::uint32_t>::size_type session::quorum()
    {
        return (mNodes.size() >> 1) + 1;
    }

    std::set<std::uint32_t>::size_type session::nodeCount()
    {
        return mNodes.size();
    }

    void session::handle(message pMsg)
    {
        switch(mState)
        {
          case state::INDUCTION:
            handleInduction(pMsg);
          case state::ELECTION:
            handleElection(pMsg);
          case state::TERMTIME:
            handleTermtime(pMsg);
        }
    }

    void session::handleInduction(message pMsg)
    {
        (void)pMsg;
    }

    void session::handleElection(message pMsg)
    {
        (void)pMsg;
    }

    void session::handleTermtime(message pMsg)
    {
        (void)pMsg;
    }

    void session::setState(state pState)
    {
        mState = pState;
    }

    bool session::seen(message pMsg)
    {
        if (std::find(mSeen.begin(), mSeen.end(), pMsg) == mSeen.end())
        {
            mSeen.push(pMsg);
            if (mSeen.size() > 512)
            {
                mSeen.pop();
            }
            return false;
        }
        else
        {
            return true;
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
