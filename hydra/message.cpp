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

        iss >> mCommand_str;

        while (iss.peek() != std::char_traits<char>::eof())
        {
            std::string param;
            iss >> param;
            mParams.push_back(param);
        }
        
        parseCommand(mCommand_str);
    }

    message::message(command pCmd, const char* pCmd_str)
        : mCommand(pCmd), mCommand_str(pCmd_str)
    {
    }

    message::message(const message& pMsg)
    {
        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;
        if (pMsg.mDerived.get())
        {
            mDerived = pMsg.mDerived;
        }
    }

    message message::operator=(const message& pMsg)
    {
        if (this != &pMsg)
        {
            mCommand = pMsg.mCommand;
            mCommand_str = pMsg.mCommand_str;
            mParams = pMsg.mParams;
            mDerived = pMsg.mDerived;
        }
        return *this;
    }

    void message::parseCommand(std::string pCommand)
    {
        if (pCommand == "PING")
        {
            mCommand = command::PING;
            mDerived = std::shared_ptr<message>(new ping(*this));
        }
        else if (pCommand == "PONG")
        {
            mCommand = command::PONG;
            mDerived = std::shared_ptr<message>(new pong(*this));
        }
        else if (pCommand == "RECV")
        {
            mCommand = command::RECV;
            mDerived = std::shared_ptr<message>(new recv(*this));
        }
        else if (pCommand == "SEND")
        {
            mCommand = command::SEND;
            mDerived = std::shared_ptr<message>(new send(*this));
        }
        else if (pCommand == "DROP")
        {
            mCommand = command::DROP;
            mDerived = std::shared_ptr<message>(new drop(*this));
        }
        else if (pCommand == "NOMINATE")
        {
            mCommand = command::NOMINATE;
            mDerived = std::shared_ptr<message>(new nominate(*this));
        }
        else if (pCommand == "PLEDGE")
        {
            mCommand = command::PLEDGE;
            mDerived = std::shared_ptr<message>(new pledge(*this));
        }
        else if (pCommand == "CALL")
        {
            mCommand = command::CALL;
            mDerived = std::shared_ptr<message>(new call(*this));
        }
        else if (pCommand == "ELECT")
        {
            mCommand = command::ELECT;
            mDerived = std::shared_ptr<message>(new elect(*this));
        }
        else if (pCommand == "KNOCK")
        {
            mCommand = command::KNOCK;
            mDerived = std::shared_ptr<message>(new knock(*this));
        }
        else if (pCommand == "MEET")
        {
            mCommand = command::MEET;
            mDerived = std::shared_ptr<message>(new meet(*this));
        }
        else if (pCommand == "WELCOME")
        {
            mCommand = command::WELCOME;
            mDerived = std::shared_ptr<message>(new welcome(*this));
        }
        else if (pCommand == "HELLO")
        {
            mCommand = command::HELLO;
            mDerived = std::shared_ptr<message>(new hello(*this));
        }
        else
        {
            mCommand = command::UNDEFINED;
            mDerived = NULL;
        }
    }

    const char* message::to_str(command pCmd) const
    {
        switch (pCmd)
        {
          case command::PING:
            return "PING";
          case command::PONG:
            return "PONG";
          case command::RECV:
            return "RECV";
          case command::SEND:
            return "SEND";
          case command::DROP:
            return "DROP";
          case command::NOMINATE:
            return "NOMINATE";
          case command::PLEDGE:
            return "PLEDGE";
          case command::CALL:
            return "CALL";
          case command::ELECT:
            return "ELECT";
          case command::KNOCK:
            return "KNOCK";
          case command::MEET:
            return "MEET";
          case command::WELCOME:
            return "WELCOME";
          case command::HELLO:
            return "HELLO";
          case command::UNDEFINED:
          default:
            return mCommand_str.c_str();
        }
    }

    bool message::operator==(const message& pMsg) const
    {
        return (mCommand == pMsg.mCommand)
            && (mParams == pMsg.mParams);
    }

    bool message::is(command pCmd)
    {
        return pCmd == mCommand;
    }
    
    std::string message::serialize() const
    {
        std::ostringstream oss;
        oss << to_str(mCommand);
        for (std::string param : mParams)
        {
            oss << ' ';
            oss << param;
        }
        return oss.str();
    }

    message message::derived()
    {
        if (mDerived.get())
        {
            return *mDerived;
        }
        else
        {
            return *this;
        }
    }

    message::~message()
    {
    }    
    
    ping::ping(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;

        iss = std::istringstream(pMsg.mParams[0]);
        iss >> term;
        iss = std::istringstream(pMsg.mParams[1]);
        iss >> val;
        iss = std::istringstream(pMsg.mParams[2]);
        iss >> id;
    }

    ping::ping(int pTerm, std::string pVal, std::uint32_t pId)
        : message(message::command::PING, "PING", pTerm, pVal, pId), term(pTerm), val(pVal), id(pId)
    {
    }

    pong::pong(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;

        iss = std::istringstream(pMsg.mParams[0]);
        iss >> val;
        iss = std::istringstream(pMsg.mParams[1]);
        iss >> id;
        iss = std::istringstream(pMsg.mParams[2]);
        iss >> count;
    }

    pong::pong(std::string pVal, std::uint32_t pId, int pCount)
        : message(message::command::PONG, "PONG", pVal, pId, pCount), val(pVal), id(pId), count(pCount)
    {
    }

    bool pong::operator==(const message& pMsg) const
    {
        return (mCommand == pMsg.mCommand)
            && (mParams[0] == pMsg.mParams[0])
            && (mParams[1] == pMsg.mParams[1]);
    }

    recv::recv(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;

        iss = std::istringstream(pMsg.mParams[0]);
        iss >> idx;
        iss = std::istringstream(pMsg.mParams[1]);
        iss >> line;
    }

    recv::recv(int pIdx, std::string pLine)
        : message(message::command::RECV, "RECV", pIdx, pLine), idx(pIdx), line(pLine)
    {
    }

    send::send(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;

        iss = std::istringstream(pMsg.mParams[0]);
        iss >> id;
        iss = std::istringstream(pMsg.mParams[1]);
        iss >> line;
    }

    send::send(std::uint32_t pId, std::string pLine)
        : message(message::command::SEND, "SEND", pId, pLine), id(pId), line(pLine)
    {
    }

    drop::drop(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;

        iss = std::istringstream(pMsg.mParams[0]);
        iss >> id;
    }

    drop::drop(std::uint32_t pId)
        : message(message::command::DROP, "DROP", pId), id(pId)
    {
    }

    nominate::nominate(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;

        iss = std::istringstream(pMsg.mParams[0]);
        iss >> term;
        iss = std::istringstream(pMsg.mParams[1]);
        iss >> id;
    }

    nominate::nominate(int pTerm, std::uint32_t pId)
        : message(message::command::NOMINATE, "NOMINATE", pTerm, pId), term(pTerm), id(pId)
    {
    }

    pledge::pledge(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;

        iss = std::istringstream(pMsg.mParams[0]);
        iss >> term;
        iss = std::istringstream(pMsg.mParams[1]);
        iss >> nom;
        iss = std::istringstream(pMsg.mParams[2]);
        iss >> id;
    }

    pledge::pledge(int pTerm, std::uint32_t pNom, std::uint32_t pId)
        : message(message::command::PLEDGE, "PLEDGE", pTerm, pNom, pId), term(pTerm), nom(pNom), id(pId)
    {
    }

    call::call(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;

        iss = std::istringstream(pMsg.mParams[0]);
        iss >> term;
    }

    call::call(int pTerm)
        : message(message::command::CALL, "CALL", pTerm), term(pTerm)
    {
    }

    elect::elect(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;
 
        iss = std::istringstream(pMsg.mParams[0]);
        iss >> term;
        iss = std::istringstream(pMsg.mParams[1]);
        iss >> nom;
        iss = std::istringstream(pMsg.mParams[2]);
        iss >> id;
    }

    elect::elect(int pTerm, std::uint32_t pNom, std::uint32_t pId)
        : message(message::command::ELECT, "ELECT", pTerm, pNom, pId), term(pTerm), nom(pNom), id(pId)
    {
    }

    knock::knock(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;
 
        iss = std::istringstream(mParams[0]);
        iss >> ind;
    }

    knock::knock(std::uint32_t pInd)
        : message(message::command::KNOCK, "KNOCK", pInd), ind(pInd)
    {
    }

    meet::meet(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;
 
        iss = std::istringstream(pMsg.mParams[0]);
        iss >> ind;
        iss = std::istringstream(pMsg.mParams[1]);
        iss >> id;
    }

    meet::meet(std::uint32_t pInd, std::uint32_t pId)
        : message(message::command::MEET, "MEET", pInd, pId), ind(pInd), id(pId)
    {
    }

    welcome::welcome(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;

        iss = std::istringstream(pMsg.mParams[0]);
        iss >> ind;
    }

    welcome::welcome(std::uint32_t pInd)
        : message(message::command::WELCOME, "WELCOME", pInd), ind(pInd)
    {
    }

    hello::hello(const message& pMsg)
        : message(pMsg)
    {
        std::istringstream iss;

        mCommand = pMsg.mCommand;
        mCommand_str = pMsg.mCommand_str;
        mParams = pMsg.mParams;
 
        iss = std::istringstream(pMsg.mParams[0]);
        iss >> ind;
        iss = std::istringstream(pMsg.mParams[1]);
        iss >> id;
    }

    hello::hello(std::uint32_t pInd, std::uint32_t pId)
        : message(message::command::HELLO, "HELLO", pInd, pId), ind(pInd), id(pId)
    {
    }
}
