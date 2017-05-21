#ifndef _HYDRA_MESSAGE_HPP_
#define _HYDRA_MESSAGE_HPP_

#include <sstream>
#include <string>
#include <vector>

#include "../config.hpp"
#include "../logger.hpp"

namespace hydra
{

    class message
    {
      public:
        enum class command
        {
            PING,
            PONG,
            RECV,
            SEND,
            DROP,
            NOMINATE,
            PLEDGE,
            CALL,
            ELECT,
            KNOCK,
            MEET,
            WELCOME,
            HELLO,
            UNDEFINED
        };

        message(std::string line);
        message(const message& pMsg);
        ~message();

        bool is(command pCmd);

      private:
        command mCommand;
        std::string mCommand_str;
        std::vector<std::string> mParams;

        void parseCommand(std::string pCommand);
    };
}

#endif /*HYDRA_MESSAGE_HPP*/
