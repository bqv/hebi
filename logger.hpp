#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <thread>

#include "config.hpp"

#define QUOTE(x) #x
#define STR(x) QUOTE(x)
#define LOC() "[" __FILE__ ":" STR(__LINE__) "] "

#define TODO throw std::runtime_error(LOC() "Not Implemented")

#ifdef DEBUG
  #define DEBUG_STREAM std::cerr
#else
  // I'm sorry
  #define DEBUG_STREAM *(new std::ostream(new NullBuffer()))
#endif

namespace logs
{
    class log_line
    {
      protected:
        log_line(std::ostream& pConsole);

        static std::vector<std::ostream*> streams;
        static std::mutex mutex;
        std::ostream& mConsole;

      public:
        static void register_stream(std::ostream* pTarget);
        static void register_streams(std::vector<std::ostream*> pTargets);

        template <typename T>
        friend log_line& operator<< (log_line& pLogLine, T pValue);

        friend log_line& operator<< (log_line& pLogLine, log_line& (&pf)(log_line&));
        friend log_line& operator<< (log_line& pLogLine, std::ostream& (&pf)(std::ostream&));

        friend log_line& done(log_line& pLogLine);
        friend log_line& endl(log_line& pLogLine);
    };

    class logger : public log_line
    {
      protected:
        char const* mLevelName;
        unsigned char mStatusCode;

      public:
        logger(std::ostream& pConsole, unsigned char pStatusCode, const char* pLevelName);

        template <typename T>
        friend log_line& operator<< (logger& pLogger, T pValue);
        friend log_line& operator<< (logger& pLogger, log_line& (&pf)(log_line&));
        friend log_line& operator<< (logger& pLogger, std::ostream& (&pf)(std::ostream&));
    };

    class NullBuffer : public std::streambuf
    {
      public:
        int overflow(int c)
        {
            return c;
        }
    };

    extern log_line& done(log_line& pLogLine);
    extern log_line& endl(log_line& pLogLine);

    extern logger fatal, error, warn, info, debug;
}

#include "thread.hpp"
#include "logger.ipp"

#endif /*LOGGER_HPP*/
