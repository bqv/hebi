#include "python.hpp"
#include <Python.h>
#include "python/plugin.h"

namespace plugin
{
    python::python(manager *pManager)
        : plugin(pManager)
    {
        logs::debug << LOC() "Initializing Python" << logs::done;
        Py_Initialize();
        PyRun_SimpleString(
            "import sys\n"
            "sys.path.append('./plugin/python/')\n"
        );
    }
    
    void python::handle(irc::message pMsg)
    {
        logs::debug << LOC() "Running Python" << logs::done;
        std::string line = pMsg.serialize();
        handle_py(line.c_str());
    }

    python::~python()
    {
        Py_Finalize();
    }
}

void send_py(const char* pLine)
{
    std::string s(pLine);
    plugin::python::send(s);
}
