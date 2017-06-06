#ifndef _HYDRA_CONNECTION_HPP_
#define _HYDRA_CONNECTION_HPP_

#include <algorithm>
#include <vector>
#include <cstdint>
#include <memory>

#include "../socket.hpp"
#include "../queue.hpp"
#include "message.hpp"

namespace hydra
{
    class session;
    
    class connection 
    {
      private:

      public:
        sockets::socket mSock;
        std::shared_ptr<session> mSess;
		std::shared_ptr<std::thread> mThreadPtr;
        queue<message> mQueue;
        std::vector<std::uint32_t> mNodes;
        bool mRunning;

        connection(sockets::socket pSock, session *pSess, std::thread *pThreadPtr);
        message read();
        bool running();
    };
}

#include "session.hpp"

#endif /*HYDRA_CONNECTION_HPP*/
