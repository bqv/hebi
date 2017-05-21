#include "message.hpp"

namespace hydra
{
    message::message(std::string line)
    {
        std::istringstream iss(line);

        if (iss.peek() == std::char_traits<char>::eof())
        {
            throw std::invalid_argument(LOC() "Message is an empty string");
        }

        std::string command;
        iss >> command;
        parseCommand(command);

        while (iss.peek() != std::char_traits<char>::eof())
        {
            std::string param;
            iss >> param;
            mParams.push_back(param);
        }
    }

    message::message(const message& pMsg)
    {
        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;
    }

    void message::parseCommand(std::string pCommand)
    {
        if (pCommand == "PING")
            mCommand = command::PING;
        else if (pCommand == "PONG")
            mCommand = command::PONG;
        else if (pCommand == "RECV")
            mCommand = command::RECV;
        else if (pCommand == "SEND")
            mCommand = command::SEND;
        else if (pCommand == "DROP")
            mCommand = command::DROP;
        else if (pCommand == "NOMINATE")
            mCommand = command::NOMINATE;
        else if (pCommand == "PLEDGE")
            mCommand = command::PLEDGE;
        else if (pCommand == "CALL")
            mCommand = command::CALL;
        else if (pCommand == "ELECT")
            mCommand = command::ELECT;
        else if (pCommand == "KNOCK")
            mCommand = command::KNOCK;
        else if (pCommand == "MEET")
            mCommand = command::MEET;
        else if (pCommand == "WELCOME")
            mCommand = command::WELCOME;
        else if (pCommand == "HELLO")
            mCommand = command::HELLO;
        else
            mCommand = command::UNDEFINED;
        mCommand_str = pCommand;
    }

    bool message::is(command pCmd)
    {
        return pCmd == mCommand;
    }

    message::~message()
    {
    }
}
