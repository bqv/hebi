#ifndef _PLUGIN_HASKELL_HPP_
#define _PLUGIN_HASKELL_HPP_

#include "../irc/message.hpp"
#include "../logger.hpp"
#include "plugin.hpp"

namespace plugin
{
    class haskell : public plugin
    {
      private:
        int *mArgc;
        char ***mArgv;

      public:
        haskell(manager *pManager, int *pArgc, char ***pArgv);
        ~haskell();

        void run() override;
    };
}

extern "C"
{
    void send_hs(const char* pLine);
}

#endif /*PLUGIN_HASKELL_HPP*/
