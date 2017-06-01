#ifndef _PLUGIN_LISP_HPP_
#define _PLUGIN_LISP_HPP_

#include "../irc/message.hpp"
#include "../logger.hpp"
#include "plugin.hpp"

namespace plugin
{
    class lisp : public plugin
    {
      private:
        int mArgc;
        char **mArgv;

      public:
        lisp(manager *pManager, int pArgc, char **pArgv);
        ~lisp();

        void run() override;
    };
}

void inner(void *data, int argc, char **argv);

#endif /*PLUGIN_LISP_HPP*/
