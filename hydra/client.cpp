#include "client.hpp"

namespace hydra
{
    client::client(sockets::socket pSock, session *pSess)
        : node(pSock), mSess(pSess), mThreadPtr(thread::make_thread_ptr("hydra::client::run", &client::run, this))
    {
		mThreadPtr->detach();
    }

    void client::run()
    {
		session& sess = *mSess;
		std::uint32_t nodeId = sess.nodeId();
        mSock.send("KNOCK %", nodeId);
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
