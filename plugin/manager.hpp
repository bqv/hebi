#ifndef _PLUGIN_MANAGER_HPP_
#define _PLUGIN_MANAGER_HPP_

#include <vector>
#include <thread>
#include <unistd.h>
#include <fcntl.h>

#include "../pipe.hpp"
#include "../irc/connection.hpp"
#include "../irc/message.hpp"
#include "haskell.hpp"
#include "python.hpp"
#include "lisp.hpp"

namespace plugin
{
    class plugin;

    class manager
    {
      private:
        std::shared_ptr<irc::connection> mConn;
        std::vector<std::shared_ptr<plugin>> mPlugins;

      public:
        manager(std::shared_ptr<irc::connection> pConn, int *pArgc, char ***pArgv);
        ~manager();

        void startPlugin(plugin *pPlg, int *pArgc, char ***pArgv);
        void watch(plugin *pPlg);
        void handle(irc::message pMsg);
        void send(irc::message pMsg);
    };
}

#include "plugin.hpp"

#endif /*PLUGIN_MANAGER_HPP*/
