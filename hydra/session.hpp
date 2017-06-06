#ifndef _HYDRA_SESSION_HPP_
#define _HYDRA_SESSION_HPP_

#include <algorithm>
#include <deque>
#include <vector>
#include <thread>
#include <cstdlib>
#include <cstdint>
#include <set>

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
        std::set<std::uint32_t>::size_type nodeCount();

      private:
        sockets::socket mSock;
		std::thread mListener;
		std::vector<server> mServers;
		std::vector<client> mClients;
        queue<message> mSeen;
        const std::uint32_t mNodeId;
        std::set<std::uint32_t> mNodes;
        state mState;

        void listen();
        void handleInduction(message pMsg);
        void handleElection(message pMsg);
        void handleTermtime(message pMsg);
        void addNode(std::uint32_t pId);
        void rmNode(std::uint32_t pId);
        std::set<std::uint32_t>::size_type quorum();
    };
}

#include "server.hpp"
#include "client.hpp"

#endif /*HYDRA_SESSION_HPP*/
