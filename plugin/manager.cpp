#include "manager.hpp"

namespace plugin
{
    manager::manager(std::shared_ptr<irc::connection> pConn, int *pArgc, char ***pArgv)
        : mConn(pConn)
    {
        *pArgc = ***pArgv;
        /*haskell *hs = new haskell(this, pArgc, pArgv);
        mPlugins.push_back(std::shared_ptr<plugin>(hs));*/
        python *py = new python(this);
        mPlugins.push_back(std::shared_ptr<plugin>(py));
    }

    void manager::handle(irc::message pMsg)
    {
        logs::debug << LOC() "Handling message" << logs::done;
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
