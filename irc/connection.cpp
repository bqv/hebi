#include "connection.hpp"

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
        mThread = thread::make_thread("irc::connection::run", &connection::run, this);
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
                        mQueue.push(msg);
                    }
                }
                catch (const std::invalid_argument&)
                {
                    logs::error << "Failed to construct message from '" << line << "'" << logs::done;
                }
            }
            lines = mSock.recv();
        }
        mRunning = false;
    }

    std::vector<message> connection::get()
    {
        std::vector<message> lines;
        while (mQueue.notEmpty())
        {
            lines.push_back(mQueue.pop());
        }
        return lines;
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

    void connection::send(irc::message pMsg)
    {
        std::string line = pMsg.serialize();
        mSock.send(line.c_str());
    }
}
