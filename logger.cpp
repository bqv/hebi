#include "logger.hpp"

namespace log
{
    logger fatal = logger(std::cerr,    00, "FATAL");
    logger error = logger(std::cerr,    10, "ERROR");
    logger warn  = logger(std::cerr,    20, "WARN");
    logger info  = logger(std::cout,    30, "INFO");
    logger debug = logger(DEBUG_STREAM, 40, "DEBUG");

    std::vector<std::ostream*> log_line::streams;
    std::mutex log_line::mutex;

    log_line::log_line(std::ostream& pConsole)
        : mConsole(pConsole)
    {
    }

    void log_line::register_stream(std::ostream* pTarget)
    {
        log_line::streams.push_back(pTarget);
    }

    void log_line::register_streams(std::vector<std::ostream*> pTargets)
    {
        for (auto target : pTargets)
        {
            log_line::streams.push_back(target);
        }
    }

    logger::logger(std::ostream& pConsole, unsigned char pStatusCode, const char* pLevelName)
        : log_line(pConsole)
        , mLevelName(pLevelName)
        , mStatusCode(pStatusCode)
    {
    }

    log_line& operator<< (log_line& pLogLine, log_line& (&pf)(log_line&))
    {
        pf(pLogLine);
        return pLogLine;
    }

    log_line& operator<< (log_line& pLogLine, std::ostream& (&pf)(std::ostream&))
    {
        pLogLine.mConsole << pf;
        for (std::ostream* streamPtr : pLogLine.streams)
            *streamPtr << pf;
        return pLogLine;
    }

    log_line& operator<< (logger& pLogger, log_line& (&pf)(log_line&))
    {
        logger::mutex.lock();

        log_line logLine = pLogger;

        logLine << pf;
        return pLogger;
    }

    log_line& done(log_line& pLogLine)
    {
        endl(pLogLine);
        log_line::mutex.unlock();
        return pLogLine;
    }

    log_line& endl(log_line& pLogLine)
    {
        pLogLine << std::endl;
        return pLogLine;
    }
};
