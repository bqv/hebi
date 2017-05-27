#include "haskell.hpp"
#include "haskell/Plugin_stub.h"

#ifdef __GLASGOW_HASKELL__
#include "Safe_stub.h"
extern void __stginit_Safe(void);
#endif

namespace plugin
{
    haskell::haskell(manager *pManager, int *pArgc, char ***pArgv)
        : plugin(pManager)
    {
        logs::debug << LOC() "Initializing Haskell" << logs::done;
        hs_init(pArgc, pArgv);
#ifdef __GLASGOW_HASKELL__
        hs_add_root(__stginit_Safe);
#endif
    }
    
    void haskell::handle(irc::message pMsg)
    {
        logs::debug << LOC() "Running Haskell" << logs::done;
        std::string line = pMsg.serialize();
        handle_hs((HsPtr) line.c_str());
    }

    haskell::~haskell()
    {
        hs_exit();
    }
}

void send_hs(const char* pLine)
{
    plugin::haskell::send(std::string(pLine));
}
