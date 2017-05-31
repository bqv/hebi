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
        int pipes[4];

        if (pipe2(&pipes[0], O_DIRECT) || pipe2(&pipes[2], O_DIRECT))
        {
            logs::error << LOC() << "Pipe failed" << logs::done;
        }

        logs::debug << "Created pipe (" << pipes[0] << " <- " << pipes[1] << ")" << logs::done;
        logs::debug << "Created pipe (" << pipes[2] << " <- " << pipes[3] << ")" << logs::done;

        pid = fork();
        if (pid == 0)
        {
            //logs::debug << "Closing file descriptor (" << pipes[1] << ")" << logs::done;
            //close(pipes[1]);
            pPlg->data_pipe = pipes::pipe(pipes[0]);
            pPlg->log_pipe = pipes::pipe(pipes[3]);
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
            //logs::debug << "Closing file descriptor (" << pipes[1] << ")" << logs::done;
            //close(pipes[0]);
            pPlg->data_pipe = pipes::pipe(pipes[1]);
            pPlg->log_pipe = pipes::pipe(pipes[2]);
            std::thread thread = thread::make_thread(pPlg->name, &manager::watch, this, pPlg);
            thread.detach();
        }
    }
 
    void manager::watch(plugin *pPlg)
    {
        for(;;)
        {
            std::string line = pPlg->log_pipe.read();
            if (!line.empty())
            {
                send(irc::message(line));
            }
        }
    }
    
    void manager::handle(irc::message pMsg)
    {
        std::string msg = pMsg.serialize();
        logs::debug << LOC() "Handling message: " << msg << logs::done;
        for (std::shared_ptr<plugin> plg : mPlugins)
        {
            logs::debug << LOC() << "Sending to " << plg->name << logs::done;
            plg->data_pipe.write(msg);
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
