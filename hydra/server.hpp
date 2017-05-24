#ifndef _HYDRA_SERVER_HPP_
#define _HYDRA_SERVER_HPP_

#include <string>
#include <vector>
#include <thread>
#include <memory>

#include "../config.hpp"
#include "../logger.hpp"
#include "../socket.hpp"
#include "../thread.hpp"
#include "node.hpp"

namespace hydra
{
    class session;

	class server : private node
	{
	  private:
        std::shared_ptr<session> mSess;
		std::shared_ptr<std::thread> mThread;

	  public:
		server(sockets::socket pSock, session *pSess);
		~server();
        void run();
        void send(const message pMsg);
        bool operator==(const server& pSrv);
	};
}

#include "session.hpp"

#endif /*HYDRA_SERVER_HPP*/
