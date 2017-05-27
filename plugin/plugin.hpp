#ifndef _PLUGIN_PLUGIN_HPP_
#define _PLUGIN_PLUGIN_HPP_

#include "../irc/message.hpp"

namespace plugin
{
    class manager;

    class plugin
    {
      private:
        static manager *mManager;

      public:
        plugin(manager *pManager);
        virtual ~plugin();
        
        virtual void handle(irc::message pMsg) = 0;
        static void send(std::string pLine);
        static void send(irc::message pMsg);
    };
}

#include "manager.hpp"

#endif /*PLUGIN_PLUGIN_HPP*/
