#include "manager.hpp"

namespace plugin
{
    manager::manager(std::shared_ptr<irc::connection> pConn, int *pArgc, char ***pArgv)
        : mConn(pConn)
    {
        haskell *hs = new haskell(this, pArgc, pArgv);
        startPlugin(hs, pArgc, pArgv);
        mPlugins.push_back(std::shared_ptr<plugin>(hs));
        python *py = new python(this);
        startPlugin(py, pArgc, pArgv);
        mPlugins.push_back(std::shared_ptr<plugin>(py));
    }

    void manager::startPlugin(plugin *pPlg, int *pArgc, char ***pArgv)
    {
        pid_t pid;
        int pipes[2];

        if (pipe2(pipes, O_DIRECT))
        {
            logs::error << LOC() << "Pipe failed" << logs::done;
        }

        pid = fork();
        if (pid == 0)
        {
            close(pipes[1]);
            pPlg->pipe = pipes::pipe(pipes[0]);
            strncpy((*pArgv)[0], pPlg->name, strlen(**pArgv));
            (*pArgv)[1] = NULL;
            *pArgc = 1;
            pPlg->run();
            exit(0);
        }
        else if (pid < 0)
        {
            logs::error << LOC() << "Fork failed" << logs::done;
        }
        else
        {
            close(pipes[0]);
            pPlg->pipe = pipes::pipe(pipes[1]);
        }
            
    }
    
    void manager::handle(irc::message pMsg)
    {
        std::string msg = pMsg.serialize();
        logs::debug << LOC() "Handling message: " << msg << logs::done;
        for (std::shared_ptr<plugin> plg : mPlugins)
        {
            plg->pipe.write(msg);
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
