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
        uint32_t id;

        message knock = node::expect(message::command::KNOCK);
        id = stol(knock.get());
        mSock.send("MEET % %", id, mSess->nodeID);
        mSess->broadcast(*this, knock);
        node::run();
    }

    void server::send(const message pMsg)
    {
        std::string msg = pMsg.serialize();
        mSock.send(msg.c_str());
    }

    bool server::operator==(const server& pSrv)
    {
        return mSock == pSrv.mSock;
    }

    server::~server()
    {
    }
}
