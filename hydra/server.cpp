#include "server.hpp"

namespace hydra
{
    server::server(sockets::socket& pSock, session *pSess)
        : connection(pSock, pSess, thread::make_thread_ptr("hydra::server::run", &server::run, this))
    {
        mClntId = 0;
    }

    void server::run()
    {
        mRunning = true;
        bool inducted = false;
        std::set<std::uint32_t> meets;

        logs::debug << LOC() "Starting server" << logs::done;

        while (mSock.connected())
        {
            message msg = connection::read();

            if (mSess->seen(msg))
            {
                logs::debug << "Ignoring seen message: " << msg.serialize() << logs::done;
                continue;
            }
            else
            {
                while (!inducted)
                {
                    if (!mClntId && msg.is(message::command::KNOCK))
                    {
                        knock knockMsg = static_cast<knock>(msg.derived());
                        mClntId = knockMsg.ind;
                        logs::debug << "Setting Induction state" << logs::done;
                        mSess->setState(session::state::INDUCTION);
                        mSess->broadcast(*this, knockMsg);
                        logs::debug << "Sending MEET" << logs::done;
                        mSock.send("MEET % %", mClntId, mSess->nodeId());
                        meets.insert(mSess->nodeId());

                        if (meets.size() == mSess->nodeCount())
                        {
                            welcome welcomeMsg(mClntId);
                            logs::debug << "Sending welcome msg: " << welcomeMsg.serialize() << logs::done;
                            mSess->broadcast(welcomeMsg);
                            meets.clear();
                            inducted = true;
                            mSock.send("HELLO % %", mClntId, mSess->nodeId());
                        }
                    }                    
                    else if (mClntId)
                    {
                        // TODO: Timeout 10s 
                        if (msg.is(message::command::MEET))
                        {
                            meet meetMsg = static_cast<meet>(msg.derived());
                            meets.insert(meetMsg.id);
                            std::string string = msg.serialize();
                            // No need to broadcast, we are the mediator
                            mSock.send(string.c_str());
                            
                            if (meets.size() == mSess->nodeCount())
                            {
                                welcome welcomeMsg(mClntId);
                                mSess->broadcast(welcomeMsg);
                                meets.clear();
                                inducted = true;
                                mSock.send("HELLO % %", mClntId, mSess->nodeId());
                            }
                        }
                        else
                        {
                            mSess->handle(msg);
                        }
                    }
                    else
                    {
                        mSess->handle(msg);
                    }
                    
                    msg = connection::read();
                }
                mSess->handle(msg);
            }
        }
        mRunning = false;

        logs::debug << LOC() "Cleaning up server" << logs::done;
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
