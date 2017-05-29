#include "manager.hpp"

namespace plugin
{
    manager::manager(std::shared_ptr<irc::connection> pConn, int *pArgc, char ***pArgv)
        : mConn(pConn)
    {
        haskell *hs = new haskell(this, pArgc, pArgv);
        mPlugins.push_back(std::shared_ptr<plugin>(hs));
        python *py = new python(this);
        mPlugins.push_back(std::shared_ptr<plugin>(py));
    }

    void manager::handle(irc::message pMsg)
    {
        logs::debug << LOC() "Handling message: " << pMsg.serialize() << logs::done;
        std::vector<std::thread*> threads;
        for (std::shared_ptr<plugin> plg : mPlugins)
        {
            std::thread *thread = thread::make_thread_ptr(plg->name, &manager::dispatch, plg, pMsg);
            threads.push_back(thread);
        }
        for (std::thread *thread : threads)
        {
            thread->join();
        }
    }
    
    void manager::send(irc::message pMsg)
    {
        mConn->send(pMsg);
    }

    void manager::dispatch(std::shared_ptr<plugin> pPlg, irc::message pMsg)
    {
        pPlg->handle(pMsg);
    }

    manager::~manager()
    {
    }
}
