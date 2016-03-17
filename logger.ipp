namespace log
{
    template <typename T>
    Logger& operator<< (Logger& log, T val)
    {
        log.console << val;
        for (std::ostream* out : log.os)
            *out << val;
        return log;
    }

    template <typename T>
    Logger& operator<< (Log& loglvl, T val)
    {
        Log::mutex.lock();

        Logger log = loglvl;

        // Todo: timestamp
        log << '+' << loglvl.num << ' ' << loglvl.str << ' ';
        log << val;
        return loglvl;
    }
};
