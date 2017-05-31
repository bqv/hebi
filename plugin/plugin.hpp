#ifndef _PLUGIN_PLUGIN_HPP_
#define _PLUGIN_PLUGIN_HPP_

#include "../pipe.hpp"
#include "../irc/message.hpp"

namespace plugin
{
    class manager;

    class plugin
    {
      private:
        static manager *mManager;

      public:
        plugin(manager *pManager, const char *pName);
        virtual ~plugin();

        virtual void run() = 0;
        static void send(std::string pLine);
        static void send(irc::message pMsg);

        const char* name;
        pipes::pipe data_pipe;
        pipes::pipe log_pipe;
    };
}

#include "manager.hpp"

#endif /*PLUGIN_PLUGIN_HPP*/
