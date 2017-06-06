#ifndef _HYDRA_SERVER_HPP_
#define _HYDRA_SERVER_HPP_

#include <string>
#include <vector>
#include <thread>
#include <memory>
#include <set>

#include "../config.hpp"
#include "../logger.hpp"
#include "../socket.hpp"
#include "../thread.hpp"
#include "message.hpp"
#include "connection.hpp"

namespace hydra
{
	class server : private connection
	{
	  private:
        std::uint32_t mClntId;

	  public:
		server(sockets::socket& pSock, session *pSess);
		~server();
        void run();
        void send(const message pMsg);
        bool operator==(const server& pSrv);
	};
}

#endif /*HYDRA_SERVER_HPP*/
