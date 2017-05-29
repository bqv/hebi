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
        else
        {
            mHostmask = NULL;
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

        mHostmask = new hostmask;
        iss.get(word, 512, '!');
        if (iss.peek() == std::char_traits<char>::eof())
        {
            mHostmask->nick = std::string();
            mHostmask->user = std::string();
            mHostmask->host = std::string(word);
        }
        else
        {
            char delim;

            mHostmask->nick = std::string(word);
            iss >> delim;
            iss.get(word, 512, '@');
            mHostmask->user = std::string(word);
            iss >> delim;
            iss.getline(word, 512);
            mHostmask->host = std::string(word);
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

    message::message(const message& obj)
    {
        if (obj.mHostmask)
        {
            mHostmask = new hostmask;
            mHostmask->nick = obj.mHostmask->nick;
            mHostmask->user = obj.mHostmask->user;
            mHostmask->host = obj.mHostmask->host;
        }
        mCommand = obj.mCommand;
        mCommand_str = obj.mCommand_str;
        mNumeric = obj.mNumeric;
        mParams = obj.mParams;
        mTrailing = obj.mTrailing;
    }

    message& message::operator=(const message& obj)
    {
        if (this != &obj)
        {
            if (obj.mHostmask)
            {
                mHostmask = new hostmask;
                mHostmask->nick = obj.mHostmask->nick;
                mHostmask->user = obj.mHostmask->user;
                mHostmask->host = obj.mHostmask->host;
            }
            mCommand = obj.mCommand;
            mCommand_str = obj.mCommand_str;
            mNumeric = obj.mNumeric;
            mParams = obj.mParams;
            mTrailing = obj.mTrailing;
        }
        return *this;
    }

    message::~message()
    {
        if (mHostmask)
        {
            delete mHostmask;
        }
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
		if (mHostmask)
		{
			oss << ':';
			oss << mHostmask->nick;
			oss << '!';
			oss << mHostmask->user;
			oss << '@';
			oss << mHostmask->host;
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
