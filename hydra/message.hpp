#ifndef _HYDRA_MESSAGE_HPP_
#define _HYDRA_MESSAGE_HPP_

#include <sstream>
#include <string>
#include <vector>
#include <cstdint>

#include "../config.hpp"
#include "../logger.hpp"
#include "../queue.hpp"

namespace hydra
{
    class ping;
    class pong;
    class recv;
    class send;
    class drop;
    class nominate;
    class pledge;
    class call;
    class elect;
    class knock;
    class meet;
    class welcome;
    class hello;

    class message
    {
        friend class ping;
        friend class pong;
        friend class recv;
        friend class send;
        friend class drop;
        friend class nominate;
        friend class pledge;
        friend class call;
        friend class elect;
        friend class knock;
        friend class meet;
        friend class welcome;
        friend class hello;
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

        template<typename T, typename... Types>
        message(command pCmd, const char* pCmd_str, T pValue, Types... pRest);
        message(command pCmd, const char* pCmd_str);
        message(std::string line);
        message(const message& pMsg);
        ~message();

        bool is(command pCmd);
        std::string get();
        std::string serialize() const;
        message derived();
        virtual bool operator==(const message& pMsg);

      protected:
        queue<std::string> mParams;
        command mCommand;
        std::string mCommand_str;
        std::shared_ptr<message> mDerived;

      private:
        void parseCommand(std::string pCommand);
    };

    class ping : public message
    {
      public:
        ping(const message& pMsg);
        ping(int pTerm, std::string pVal, std::uint32_t pId);

        int term;
        std::string val;
        std::uint32_t id;
    };

    class pong : public message
    {
      public:
        pong(const message& pMsg);
        pong(std::string pVal, std::uint32_t pId, int pCount);

        std::string val;
        std::uint32_t id;
        int count;
    };

    class recv : public message
    {
      public:
        recv(const message& pMsg);
        recv(int pIdx, std::string pLine);

        int idx;
        std::string line;
    };

    class send : public message
    {
      public:
        send(const message& pMsg);
        send(std::uint32_t pId, std::string pLine);

        std::uint32_t id;
        std::string line;
    };

    class drop : public message
    {
      public:
        drop(const message& pMsg);
        drop(std::uint32_t pId);

        std::uint32_t id;
    };

    class nominate : public message
    {
      public:
        nominate(const message& pMsg);
        nominate(int pTerm, std::uint32_t pId);

        int term;
        std::uint32_t id;
    };

    class pledge : public message
    {
      public:
        pledge(const message& pMsg);
        pledge(int pTerm, std::uint32_t pNom, std::uint32_t pId);

        int term;
        std::uint32_t nom;
        std::uint32_t id;
    };

    class call : public message
    {
      public:
        call(const message& pMsg);
        call(int pTerm);

        int term;
    };

    class elect : public message
    {
      public:
        elect(const message& pMsg);
        elect(int pTerm, std::uint32_t pNom, std::uint32_t pId);

        int term;
        std::uint32_t nom;
        std::uint32_t id;
    };

    class knock : public message
    {
      public:
        knock(const message& pMsg);
        knock(std::uint32_t pId);

        std::uint32_t id;
    };

    class meet : public message
    {
      public:
        meet(const message& pMsg);
        meet(std::uint32_t pId, std::uint32_t pMed);

        std::uint32_t id;
        std::uint32_t med;
    };

    class welcome : public message
    {
      public:
        welcome(const message& pMsg);
        welcome(std::uint32_t pId);

        std::uint32_t id;
    };

    class hello : public message
    {
      public:
        hello(const message& pMsg);
        hello(std::uint32_t pId);

        std::uint32_t id;
    };
}

#include "message.ipp"

#endif /*HYDRA_MESSAGE_HPP*/
