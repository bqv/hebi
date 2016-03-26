#include "connection.hpp"
#include <chrono>
#include <thread>

namespace irc
{
    connection::connection(const std::string pHost, unsigned short pPort)
        : mSock(pHost.c_str(), pPort)
    {
    }

    connection::~connection()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        this->quit("Connection closing...");
    }

    void connection::run()
    {
        this->do_register(NICK);
        this->join("#programming");

        while (std::vector<std::string> lines = mSock.recv())
        {
            std::lock_guard<std::mutex> guard(mLock);
            for (std::string line : lines)
            {
                mQueue.push(line);
            }
        }
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
