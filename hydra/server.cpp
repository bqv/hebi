#include "server.hpp"

namespace hydra
{
    server::server(sockets::socket pSock, session *pSess)
        : node(pSock)
    {
        mSess = std::shared_ptr<session>(pSess);
        mThread = std::shared_ptr<std::thread>(new std::thread(&server::run, this));
    }

    void server::run()
    {
        /* Wait for knocks */
    }

    server::~server()
    {
    }
}
