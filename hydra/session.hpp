#ifndef _HYDRA_SESSION_HPP_
#define _HYDRA_SESSION_HPP_

#include <algorithm>
#include <deque>
#include <vector>
#include <thread>
#include <cstdlib>
#include <cstdint>

#include "../config.hpp"
#include "../logger.hpp"
#include "../socket.hpp"
#include "message.hpp"

namespace hydra
{
	class server;

	class client;

    class session
    {
      private:
        sockets::socket mSock;
		std::thread mListener;
		std::vector<server> mServers;
		std::vector<client> mClients;
        std::deque<message> mSeen;

      public:
        session(unsigned short pPort);
        ~session();
        void connect(const std::string pHost, unsigned short pPort);
        void listen();
        void broadcast(server& pSrv, message pMsg);
        void broadcast(client& pClnt, message pMsg);

        const std::uint32_t nodeID;
    };
}

#include "server.hpp"
#include "client.hpp"

#endif /*HYDRA_SESSION_HPP*/
