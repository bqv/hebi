#include "session.hpp"

namespace hydra
{
    session::session(unsigned short pPort)
        : mSock(pPort), mNodeId(rand()), mNodes({mNodeId})
    {
        mState = state::TERMTIME;
        setLeader(mNodeId);
        mInductee = 0;
        mPingVal = 0;
        mTerm = 0;
		mListener = thread::make_thread("hydra::session::listen", &session::listen, this);
		mListener.detach();
        mPingTimer = thread::make_thread("hydra::session::pingTimer", &session::pingTimer, this);
        mPingTimer.detach();
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

    void session::pingTimer()
    {
        for(;;)
        {
            std::ostringstream oss;
            oss << ++mPingVal;
            mPingVal_str = oss.str();
            {
                std::unique_lock<std::mutex> lock(mLeaderLock);
                while (mLeader != mNodeId)
                {
                    mIsNotLeader.wait(lock);
                }
                ping pingMsg(mTerm, mPingVal_str, mNodeId);
                broadcast(pingMsg);
                mPongs.insert(mNodeId);
            }
            std::this_thread::sleep_for(std::chrono::seconds(MAX_PING));
            
            std::set<std::uint32_t> difference;
            std::set_difference(mNodes.begin(), mNodes.end(), mPongs.begin(), mPongs.end(), std::inserter(difference, difference.end()));
            for (std::uint32_t id : difference)
            {
                logs::debug << "Node " << id << " failed to respond to ping in time, dropping..." << logs::done;
                drop dropMsg(id);
                broadcast(dropMsg);
                rmNode(id);
            }
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
        if (pMsg.is(message::command::WELCOME))
        {
            welcome welcomeMsg = static_cast<welcome>(pMsg.derived());
            if (welcomeMsg.ind != mInductee)
            {
                return;
            }
            addNode(mInductee);
            mInductee = 0;
            setState(state::TERMTIME);
        }
    }

    void session::handleElection(message pMsg)
    {
        (void)pMsg;
    }

    void session::handleTermtime(message pMsg)
    {
        if (pMsg.is(message::command::KNOCK))
        {
            knock knockMsg = static_cast<knock>(pMsg.derived());
            mInductee = knockMsg.ind;
            setState(state::INDUCTION);
            meet meetMsg(mInductee, mNodeId);
            broadcast(meetMsg);
        }
        else if (mLeader == mNodeId)
        {
            if (pMsg.is(message::command::PING))
            {
                ping pingMsg = static_cast<ping>(pMsg.derived());
                if (pingMsg.term > mTerm)
                {
                    setLeader(pingMsg.id);
                }
                else if (pingMsg.term == mTerm)
                {
                    setState(state::ELECTION);
                    nominate nominateMsg(++mTerm, mNodeId);
                    broadcast(nominateMsg);
                }
            }
            else if (pMsg.is(message::command::PONG))
            {
                pong pongMsg = static_cast<pong>(pMsg.derived());
                // No need to relay, we are the final destination
                if (pongMsg.val == mPingVal_str)
                {
                    mPongs.insert(pongMsg.id);
                }
            }
            else if (pMsg.is(message::command::SEND))
            {
                // TODO: Forward to IRC
            }
            else if (pMsg.is(message::command::DROP))
            {
                drop dropMsg = static_cast<drop>(pMsg.derived());
                rmNode(dropMsg.id);
            }
        }
        else
        {
            if (pMsg.is(message::command::PING))
            {
                ping pingMsg = static_cast<ping>(pMsg.derived());
                if (pingMsg.term > mTerm)
                {
                    mTerm = pingMsg.term;
                    setLeader(pingMsg.id);
                }
                pong pongMsg(pingMsg.val, pingMsg.id, 1);
                broadcast(pongMsg);
            }
            if (pMsg.is(message::command::PONG))
            {
                pong pongMsg_in = static_cast<pong>(pMsg.derived());
                pong pongMsg_out(pongMsg_in.val, pongMsg_in.id, pongMsg_in.count+1);
                broadcast(pongMsg_out);
            }
        }
    }

    void session::setLeader(std::uint32_t pId)
    { 
        std::unique_lock<std::mutex> lock(mLeaderLock);
        mLeader = pId;
        if (mLeader == mNodeId)
        {
            mIsNotLeader.notify_all();
        }
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
