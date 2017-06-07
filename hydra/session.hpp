#ifndef _HYDRA_SESSION_HPP_
#define _HYDRA_SESSION_HPP_

#include <deque>
#include <vector>
#include <thread>
#include <cstdlib>
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <set>
#include <algorithm>
#include <iterator>

#include "../config.hpp"
#include "../logger.hpp"
#include "../socket.hpp"
#include "../thread.hpp"
#include "message.hpp"

namespace hydra
{
	class server;

	class client;

    class session
    {
      public:
        enum class state
        {
            INDUCTION,
            ELECTION,
            TERMTIME
        };

        session(unsigned short pPort);
        ~session();
        void connect(const std::string pHost, unsigned short pPort);
        void broadcast(server& pSrv, message pMsg);
        void broadcast(client& pClnt, message pMsg);
        void broadcast(message pMsg);
        void handle(message pMsg);	
        void setState(state pState);
        bool seen(message pMsg);
        std::uint32_t nodeId();
        void addNode(std::uint32_t pId);
        void rmNode(std::uint32_t pId);
        std::set<std::uint32_t>::size_type nodeCount();

      private:
        sockets::socket mSock;
		std::thread mListener;
		std::thread mPingTimer;
		std::vector<server> mServers;
		std::vector<client> mClients;
        queue<message> mSeen;
        const std::uint32_t mNodeId;
        std::set<std::uint32_t> mNodes;
        state mState;
        std::uint32_t mLeader;
        int mTerm;
        std::uint32_t mInductee;
        unsigned mPingVal;
        std::string mPingVal_str;
        std::set<std::uint32_t> mPongs;
        std::mutex mLeaderLock;
        std::condition_variable mIsNotLeader;

        void listen();
        void pingTimer();
        void handleInduction(message pMsg);
        void handleElection(message pMsg);
        void handleTermtime(message pMsg);
        void setLeader(std::uint32_t pId);
        std::set<std::uint32_t>::size_type quorum();
    };
}

#include "server.hpp"
#include "client.hpp"

#endif /*HYDRA_SESSION_HPP*/
