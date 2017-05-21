#ifndef _HYDRA_SESSION_HPP_
#define _HYDRA_SESSION_HPP_

#include <vector>
#include <thread>

#include "../config.hpp"
#include "../logger.hpp"
#include "../socket.hpp"

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

      public:
        session(unsigned short pPort);
        ~session();
        void connect(const std::string pHost, unsigned short pPort);
        void listen();
    };
}

#include "server.hpp"
#include "client.hpp"

#endif /*HYDRA_SESSION_HPP*/
