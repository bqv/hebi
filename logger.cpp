#include "logger.hpp"

namespace log
{
    Log fatal = Log(std::cerr, 00, "FATAL");
    Log error = Log(std::cerr, 10, "ERROR");
    Log warn  = Log(std::cerr, 20, "WARN");
    Log info  = Log(std::cout, 30, "INFO");
    Log debug = Log(std::cerr, 40, "DEBUG");

    std::vector<std::ostream*> Logger::os;
    std::mutex Logger::mutex;

    Logger::Logger(std::ostream& console)
        : console(console)
    {
    }

    Logger::~Logger()
    {
    }

    void Logger::tee(std::vector<std::ostream*> targets)
    {
        for (auto os : targets)
            Logger::os.push_back(os);
    }

    Log::Log(std::ostream& console, short int num, const char* str)
        : Logger(console)
        , str(str)
        , num(num)
    {
    }

    Log::~Log()
    {
    }

    Logger& operator<< (Logger& log, Logger& (&f)(Logger&))
    {
        return f(log);
    }

    Logger& operator<< (Logger& log, std::ostream& (&f)(std::ostream&))
    {
        log.console << f;
        for (std::ostream* out : log.os)
            *out << f;
        return log;
    }

    Logger& operator<< (Log& loglvl, Logger& (&f)(Logger&))
    {
        Log::mutex.lock();

        Logger log = loglvl;

        log << f;
        return loglvl;
    }

    Logger& done(Logger& log)
    {
        endl(log);
        Logger::mutex.unlock();
        return log;
    }

    Logger& endl(Logger& log)
    {
        return log << std::endl;
    }
};
