#include "client.hpp"

namespace hydra
{
    client::client(sockets::socket pSock, session *pSess)
        : node(pSock)
    {
        mSess = std::shared_ptr<session>(pSess);
        mThread = std::shared_ptr<std::thread>(thread::make_thread_ptr("hydra::client::run", &client::run, this));
    }

    void client::run()
    {
        mSock.send("KNOCK %", mSess->nodeID);
        message msg = node::expect(message::command::MEET);
        meet meetMsg = (meet) msg.derived();
        std::uint32_t mediator_id = meetMsg.med;
        node::addNode(mediator_id);
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
