#ifndef _HYDRA_BASE_HPP_
#define _HYDRA_BASE_HPP_

#include "../socket.hpp"
#include "../queue.hpp"
#include "message.hpp"

namespace hydra
{
    class node
    {
      private:

      public:
        sockets::socket mSock;
        queue<message> mQueue;

        node(sockets::socket pSock);
        message read();
        message expect(message::command pCmd);
        void run();
    };
}

#endif /*HYDRA_BASE_HPP*/
