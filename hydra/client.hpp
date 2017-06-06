#ifndef _HYDRA_CLIENT_HPP_
#define _HYDRA_CLIENT_HPP_

#include <string>
#include <vector>
#include <thread>
#include <memory>

#include "../config.hpp"
#include "../logger.hpp"
#include "../socket.hpp"
#include "../thread.hpp"
#include "message.hpp"
#include "connection.hpp"

namespace hydra
{
	class client : private connection
	{
	  private:
        std::uint32_t mSrvId;

	  public:
		client(sockets::socket& pSock, session *pSess);
		~client();
        void run();
        void send(const message pMsg);
        bool operator==(const client& pSrv);
	};
}

#endif /*HYDRA_CLIENT_HPP*/
