#include "connection.hpp"
#include <chrono>

namespace irc
{
    connection::connection(const std::string pHost, unsigned short pPort)
        : mSock(pHost.c_str(), pPort)
    {
    }

    connection::~connection()
    {
    }

    void connection::start()
    {
        mThread = std::thread(&connection::run, this);
        mThread.detach();
        mRunning = true;
    }

    void connection::stop()
    {
        this->quit("Connection closing...");
        mSock.close();
        mRunning = false;
    }

    void connection::run()
    {
        this->do_register(NICK);
        this->join("##doge");

        std::vector<std::string> lines = mSock.recv();
        while (mSock.connected())
        {
            for (std::string line : lines)
            {
                try
                {
                    message msg(line);
                    if (msg.isPing())
                    {
                        this->pong(msg.all_params());
                    }
                    else
                    {
                        std::lock_guard<std::mutex> guard(mLock);
                        mQueue.push(msg);
                    }
                }
                catch (const std::invalid_argument&)
                {
                    log::error << "Failed to construct message from '" << line << "'";
                }
            }
            lines = mSock.recv();
        }
        mRunning = false;
    }

    message connection::get()
    {
        while (mQueue.empty())
        {
        }
        std::lock_guard<std::mutex> guard(mLock);
        message msg = mQueue.front();
        mQueue.pop();
        return msg;
    }

    bool connection::running()
    {
        return mRunning;
    }

    void connection::do_register(std::string pNick)
	{
        this->user("Hydra", "*", "*", "Hydra decentralised IRC bot");
        this->nick(pNick);
	}

    bool connection::user(std::string pUser, std::string pHost, std::string pServer, std::string pRealName)
    {
        mSock.send("USER % % % %", pUser, pHost, pServer, pRealName);
        return true;
    }

    bool connection::nick(std::string pNick)
    {
        mSock.send("NICK %", pNick);
        return true;
    }

    bool connection::pong(std::string pPing)
	{
        mSock.send("PONG %", pPing);
        return true;
	}

    bool connection::join(std::vector<std::string> pChans)
	{
        for (std::string chan : pChans)
        {
            this->join(chan);
        }
        return true;
	}

    bool connection::join(std::string pChan)
	{
        mSock.send("JOIN %", pChan);
        return true;
	}

    bool connection::quit(std::string pMsg)
	{
        mSock.send("QUIT %", pMsg);
        return true;
	}

    bool connection::part(std::string pChan, std::string pMsg)
	{
        mSock.send("PART % :%", pChan, pMsg);
        return true;
	}

    bool connection::privmsg(std::string pChan, std::string pMsg)
	{
        mSock.send("PRIVMSG % :%", pChan, pMsg);
        return true;
	}

    bool connection::notice(std::string pChan, std::string pMsg)
	{
        mSock.send("NOTICE % :%", pChan, pMsg);
        return true;
	}

}
