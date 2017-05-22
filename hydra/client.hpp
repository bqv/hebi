#ifndef _HYDRA_CLIENT_HPP_
#define _HYDRA_CLIENT_HPP_

#include <string>
#include <vector>
#include <thread>
#include <memory>

#include "../config.hpp"
#include "../logger.hpp"
#include "../socket.hpp"
#include "node.hpp"

namespace hydra
{
    class session;

	class client : private node
	{
	  private:
        std::shared_ptr<session> mSess;
		std::shared_ptr<std::thread> mThread;

	  public:
		client(sockets::socket pSock, session *pSess);
		~client();
        void run();
        void send(const message pMsg);
        bool operator==(const client& pSrv);
	};
}

#include "session.hpp"

#endif /*HYDRA_CLIENT_HPP*/
