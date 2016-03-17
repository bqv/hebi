#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <iostream>
#include <memory>
#include <vector>
#include <mutex>

namespace log
{
    class Logger
    {
      protected:
        Logger(std::ostream& console);

        static std::vector<std::ostream*> os;
        static std::mutex mutex;
        std::ostream& console;

      public:
        virtual ~Logger();
        
        static void tee(const std::vector<std::ostream*> targets);

        template <typename T>
        friend Logger& operator<< (Logger& log, T val);
        friend Logger& operator<< (Logger& log, Logger& (&f)(Logger&));
        friend Logger& operator<< (Logger& log, std::ostream& (&f)(std::ostream&));

        friend Logger& done(Logger& log);
        friend Logger& endl(Logger& log);
    };

    class Log : public Logger
    {
      protected:
        char const* str;
        short int num;

      public:
        Log(std::ostream& console, short int num, const char* str);
        virtual ~Log();

        template <typename T>
        friend Logger& operator<< (Log& log, T val);
        friend Logger& operator<< (Log& log, Logger& (&f)(Logger&));
        friend Logger& operator<< (Log& log, std::ostream& (&f)(std::ostream&));
    };

    extern Logger& done(Logger& log);
    extern Logger& endl(Logger& log);

    extern Log fatal, error, warn, info, debug;
};

#include "logger.ipp"

#endif /*LOGGER_HPP*/
