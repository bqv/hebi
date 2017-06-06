#include "connection.hpp"

namespace hydra
{
    connection::connection(sockets::socket pSock, session *pSess, std::thread *pThreadPtr)
        : mSock(pSock), mSess(pSess), mThreadPtr(pThreadPtr)
    {
        mThreadPtr->detach();
    }

    message connection::read()
    {
        while (mQueue.empty())
        {
            std::vector<std::string> lines = mSock.recv();
            for (std::string line : lines)
            {
                try
                {
                    logs::debug << "Reading line: " << line << logs::done;
                    message msg = message(line);
                    logs::debug << "Read message: " << msg.serialize() << logs::done;
                    mQueue.push(msg);
                }
                catch (const std::invalid_argument&)
                {
                    logs::error << "Error interpreting message: " << line << logs::done;
                }
            }
        }
        return mQueue.pop();
    }

    bool connection::running()
    {
        return mRunning;
    }
}
