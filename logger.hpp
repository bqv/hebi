#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <iostream>
#include <mutex>
#include <vector>

namespace log
{
    class log_line
    {
      protected:
        log_line(std::ostream& pConsole);

        static std::vector<std::ostream*> streams;
        static std::mutex mutex;
        std::ostream& mConsole;

      public:
        virtual ~log_line();
        
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
        short int mStatusCode;

      public:
        logger(std::ostream& pConsole, short int pStatusCode, const char* pLevelName);
        virtual ~logger();

        template <typename T>
        friend log_line& operator<< (logger& pLogger, T pValue);
        friend log_line& operator<< (logger& pLogger, log_line& (&pf)(log_line&));
        friend log_line& operator<< (logger& pLogger, std::ostream& (&pf)(std::ostream&));
    };

    extern log_line& done(log_line& pLogLine);
    extern log_line& endl(log_line& pLogLine);

    extern logger fatal, error, warn, info, debug;
};

#include "logger.ipp"

#endif /*LOGGER_HPP*/
