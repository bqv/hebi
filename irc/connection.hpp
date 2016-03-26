#ifndef _IRC_CONNECTION_HPP_
#define _IRC_CONNECTION_HPP_

#include <queue>
#include <string>
#include <vector>

#include "../config.hpp"
#include "../logger.hpp"
#include "socket.hpp"

namespace irc
{
    class connection
    {
      private:
        bool user(std::string pUser, std::string pHost, std::string pServer, std::string pRealName);
        bool quit(std::string pMsg);
        void do_register(std::string pNick);

        std::mutex mLock;
        std::queue<std::string> mQueue;
        socket mSock;

      public:
        connection(const std::string pHost, unsigned short pPort);
        ~connection();

        void run();
        std::string get();

        bool nick(std::string pNick);
        bool join(std::vector<std::string> pChans);
        bool join(std::string pChan);
        bool part(std::string pChan, std::string pMsg);
        bool privmsg(std::string pChan, std::string pMsg);
        bool notice(std::string pChan, std::string pMsg);
    };
}

#endif /*IRC_CONNECTION_HPP*/
