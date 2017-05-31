#include "python.hpp"
#include <Python.h>
#include "python/plugin.h"

namespace plugin
{
    python::python(manager *pManager)
        : plugin(pManager, "plugin::python")
    {
    }

    void python::run()
    {
        logs::debug << LOC() "Initializing Python" << logs::done;
        Py_Initialize();
        PyRun_SimpleString(
            "import sys\n"
            "sys.path.append('./plugin/python/')\n"
        );
        PyInit_plugin();
        run_py(data_pipe.fd(), log_pipe.fd());
    }

    python::~python()
    {
        logs::debug << LOC() "Shutting Down Python" << logs::done;
        Py_Finalize();
    }
}

void send_py(const char* pLine)
{
    std::string s(pLine);
    plugin::python::send(s);
}
