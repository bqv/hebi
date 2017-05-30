#include "haskell.hpp"
#include "haskell/Plugin_stub.h"

#ifdef __GLASGOW_HASKELL__
#include "Safe_stub.h"
extern void __stginit_Safe(void);
#endif

namespace plugin
{
    haskell::haskell(manager *pManager, int *pArgc, char ***pArgv)
        : plugin(pManager, "plugin::haskell")
    {
        mArgc = pArgc;
        mArgv = pArgv;
    }

    void haskell::run()
    {
        logs::debug << LOC() "Initializing Haskell" << logs::done;
        hs_init(mArgc, mArgv);
#ifdef __GLASGOW_HASKELL__
        hs_add_root(__stginit_Safe);
#endif
        run_hs(pipe.fd());
    }
    
    void haskell::handle(irc::message pMsg)
    {
        logs::debug << LOC() "Running Haskell" << logs::done;
        std::string line = pMsg.serialize();
        handle_hs((HsPtr) line.c_str());
    }

    haskell::~haskell()
    {
        logs::debug << LOC() "Shutting Down Haskell" << logs::done;
        hs_exit();
    }
}

void send_hs(const char* pLine)
{
    std::string s(pLine);
    plugin::haskell::send(s);
}

void log_debug_hs(const char* pLine)
{
    logs::debug << pLine << logs::done;
}

void log_info_hs(const char* pLine)
{
    logs::info << pLine << logs::done;
}

void log_warn_hs(const char* pLine)
{
    logs::warn << pLine << logs::done;
}

void log_error_hs(const char* pLine)
{
    logs::error << pLine << logs::done;
}

void log_fatal_hs(const char* pLine)
{
    logs::fatal << pLine << logs::done;
    exit(-1);
}
