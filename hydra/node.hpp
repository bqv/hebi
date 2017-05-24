#ifndef _HYDRA_BASE_HPP_
#define _HYDRA_BASE_HPP_

#include <algorithm>
#include <vector>
#include <cstdint>

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
        std::vector<std::uint32_t> mNodes;

        node(sockets::socket pSock);
        message read();
        message expect(message::command pCmd);
        void addNode(std::uint32_t pId);
        void rmNode(std::uint32_t pId);
        void run();
    };
}

#endif /*HYDRA_BASE_HPP*/
