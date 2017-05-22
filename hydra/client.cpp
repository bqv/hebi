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
        uint32_t med;

        mSock.send("KNOCK %", mSess->nodeID);
        message msg = node::expect(message::command::MEET);
        med = stol(msg.get());
        node::run();
    }

    void client::send(const message pMsg)
    {
        std::string msg = pMsg.serialize();
        mSock.send(msg.c_str());
    }

    bool client::operator==(const client& pSrv)
    {
        return mSock == pSrv.mSock;
    }

    client::~client()
    {
    }
}
