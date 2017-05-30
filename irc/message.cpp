#include "message.hpp"

namespace irc
{
    message::message(std::string line)
    {
        std::istringstream iss(line);

        if (iss.peek() == std::char_traits<char>::eof())
        {
            throw std::invalid_argument(LOC() "Message is an empty string");
        }

        if (iss.peek() == ':')
        {
            std::string hostmask;
            char colon;

            iss >> colon;
            iss >> hostmask;
            parseHostmask(hostmask);
        }

        std::string command;
        iss >> command;
        parseCommand(command);

        while (iss.peek() != std::char_traits<char>::eof())
        {
            std::string param;
            iss >> param;
            if (param[0] == ':')
            {
                std::ostringstream oss;
                char rest[512] = {0};

                oss << param.substr(1);
                iss.getline(rest, 512);
                oss << rest;
                mTrailing = oss.str();
            }
            else
            {
                mParams.push_back(param);
            }
        }
    }

    void message::parseHostmask(std::string pHostmask)
    {
        std::istringstream iss(pHostmask);
        char word[512];

        iss.get(word, 512, '!');
        if (iss.peek() == std::char_traits<char>::eof())
        {
            mHostmask.host = std::string(word);
        }
        else
        {
            char delim;

            mHostmask.nick = std::string(word);
            iss >> delim;
            iss.get(word, 512, '@');
            mHostmask.user = std::string(word);
            iss >> delim;
            iss.getline(word, 512);
            mHostmask.host = std::string(word);
        }
    }

    void message::parseCommand(std::string pCommand)
    {
        try
        {
            mNumeric = stoi(pCommand);
            mCommand = command::NUMERIC;
        }
        catch (const std::invalid_argument&)
        {
            if (pCommand == "PING")
                mCommand = command::PING;
            else if (pCommand == "PRIVMSG")
                mCommand = command::PRIVMSG;
            else if (pCommand == "NOTICE")
                mCommand = command::NOTICE;
            else if (pCommand == "INVITE")
                mCommand = command::INVITE;
            else
                mCommand = command::UNDEFINED;
        }
        mCommand_str = pCommand;
    }

    message::~message()
    {
    }

    bool message::isPing()
    {
        return mCommand == command::PING;
    }

    std::string message::all_params()
    {
        std::ostringstream oss;
        for (std::string param : mParams)
        {
            oss << param;
            oss << ' ';
        }
        if (!mTrailing.empty())
        {
            oss << ':';
            oss << mTrailing;
        }
        return oss.str();
    }
    
    std::string message::serialize() const
    {
        std::ostringstream oss;
		if (!mHostmask.host.empty())
		{
			oss << ':';
            if (!mHostmask.nick.empty())
            {
                oss << mHostmask.nick;
                oss << '!';
                oss << mHostmask.user;
                oss << '@';
            }
			oss << mHostmask.host;
			oss << ' ';
		}
        oss << mCommand_str;
        for (std::string param : mParams)
        {
            oss << ' ';
            oss << param;
        }
		if (!mTrailing.empty())
		{
			oss << " :";
			oss << mTrailing;
		}
        return oss.str();
    }
}
