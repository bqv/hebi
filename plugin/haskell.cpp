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
        run_hs(data_pipe.fd(), log_pipe.fd());
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

