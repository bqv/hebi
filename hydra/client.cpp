#include "client.hpp"

namespace hydra
{
    client::client(sockets::socket pSock, session *pSess)
        : node(pSock)
    {
        mSess = std::shared_ptr<session>(pSess);
        mThread = std::shared_ptr<std::thread>(new std::thread(&client::run, this));
    }

    void client::run()
    {
        /* Knock and begin induction */
    }

    client::~client()
    {
    }
}
