#include "node.hpp"

namespace hydra
{
    node::node(sockets::socket pSock)
        : mSock(pSock)
    {
    }
    
    message node::expect(message::command pCmd)
    {
        std::vector<std::string> lines;
        
        while (mSock.connected())
        {
            lines = mSock.recv();
            for (auto line : lines)
            {
                try
                {
                    message msg = message(line);
                    mQueue.push(msg);
                }
                catch (const std::invalid_argument&)
                {
                }
            }
            while (mQueue.notEmpty())
            {
                message msg = mQueue.pop();
                if (msg.is(pCmd))
                {
                    return msg;
                }
            }
        }
        return message("");
    }
}
