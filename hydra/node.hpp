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
        message expect(message::command pCmd);
    };
}

#endif /*HYDRA_BASE_HPP*/
