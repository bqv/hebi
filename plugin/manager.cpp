#include "manager.hpp"

namespace plugin
{
    manager::manager(std::shared_ptr<irc::connection> pConn, int *pArgc, char ***pArgv)
        : mConn(pConn)
    {
        haskell *hs = new haskell(this, pArgc, pArgv);
        mPlugins.push_back(std::shared_ptr<plugin>(hs));
    }

    void manager::handle(irc::message pMsg)
    {
        log::debug << LOC() "Handling message" << log::done;
        for (std::shared_ptr<plugin> plg : mPlugins)
        {
            plg->handle(pMsg);
        }
    }
    
    void manager::send(irc::message pMsg)
    {
        mConn->send(pMsg);
    }

    manager::~manager()
    {
    }
}
