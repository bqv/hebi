#ifndef _PLUGIN_PYTHON_HPP_
#define _PLUGIN_PYTHON_HPP_

#ifdef __cplusplus
#include "../irc/message.hpp"
#include "../logger.hpp"
#include "plugin.hpp"

namespace plugin
{
    class python : public plugin
    {
      private:

      public:
        python(manager *pManager);
        ~python();

        void handle(irc::message pMsg) override;
    };
}

extern "C" {
#endif
    void send_py(const char* pLine);
#ifdef __cplusplus
}
#endif

#endif /*PLUGIN_PYTHON_HPP*/
