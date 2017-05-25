#include "server.hpp"

namespace hydra
{
    server::server(sockets::socket pSock, session *pSess)
        : node(pSock), mSess(pSess)
    {
        mThreadPtr = std::shared_ptr<std::thread>(thread::make_thread_ptr("hydra::server::run", &server::run, this));
		mThreadPtr->detach();
    }

    void server::run()
    {
        for(;;)
        {
            message msg = node::read();
            if (msg.is(message::command::KNOCK))
            {
                knock knockMsg = (knock) msg.derived();
                std::uint32_t client_id = knockMsg.id;
                mSock.send("MEET % %", client_id, mSess->nodeId());
                mSess->broadcast(*this, knockMsg);
                node::run();
            }
        }
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
