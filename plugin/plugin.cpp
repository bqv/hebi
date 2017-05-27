#include "plugin.hpp"

namespace plugin
{
    plugin::plugin(manager *pManager)
    {
        mManager = pManager;
    }

    void plugin::send(std::string pLine)
    {
        mManager->send(irc::message(pLine));
    }

    void plugin::send(irc::message pMsg)
    {
        mManager->send(pMsg);
    }

    plugin::~plugin()
    {
    }

    manager *plugin::mManager;
}
