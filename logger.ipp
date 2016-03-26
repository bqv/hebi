namespace log
{
    template <typename T>
    log_line& operator<< (log_line& pLogLine, T pValue)
    {
        pLogLine.mConsole << pValue;
        for (std::ostream* streamPtr : pLogLine.streams)
        {
            *streamPtr << pValue;
        }
        return pLogLine;
    }

    template <typename T>
    log_line& operator<< (logger& pLogger, T pValue)
    {
        logger::mutex.lock();

        log_line logLine = pLogger;

        // Todo: timestamp
        logLine << '+' << +pLogger.mStatusCode << ' ' << pLogger.mLevelName << ' ';
        logLine << pValue;
        return pLogger;
    }
};
