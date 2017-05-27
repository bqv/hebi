#ifndef _IRC_MESSAGE_HPP_
#define _IRC_MESSAGE_HPP_

#include <sstream>
#include <string>
#include <vector>

#include "../config.hpp"
#include "../logger.hpp"

namespace irc
{

    class message
    {
      protected:
        struct hostmask
        {
            std::string nick;
            std::string user;
            std::string host;
        };

        enum class command
        {
            NUMERIC,
            PING,
            PRIVMSG,
            NOTICE,
            INVITE,
            UNDEFINED
        };

      private:
        hostmask* mHostmask;
        command mCommand;
        std::string mCommand_str;
        int mNumeric;
        std::vector<std::string> mParams;
        std::string mTrailing;

        void parseHostmask(std::string pHostmask);
        void parseCommand(std::string pCommand);

      public:
        message(std::string line);
        message(const message& obj);
        ~message();
        message& operator=(const message& obj);
        bool isPing();
        std::string all_params();
		std::string serialize() const;
    };
}

#endif /*IRC_MESSAGE_HPP*/
