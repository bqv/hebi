#include "lisp.hpp"
#include <libguile.h>

namespace plugin
{
    lisp::lisp(manager *pManager, int pArgc, char **pArgv)
        : plugin(pManager, "plugin::lisp")
    {
        mArgc = pArgc - pArgc;
        mArgv = pArgv;
    }

    void lisp::run()
    {
        logs::debug << LOC() "Initializing Lisp" << logs::done;
        int data[2];
        data[0] = data_pipe.fd();
        data[1] = log_pipe.fd();
        scm_boot_guile(mArgc, mArgv, &inner, data);
    }

    lisp::~lisp()
    {
        logs::debug << LOC() "Shutting Down Lisp" << logs::done;
    }
}

SCM send_ls(SCM pLine)
{
    const char *line = scm_to_locale_string(pLine);
    std::string s(line);
    plugin::lisp::send(s);
    return SCM_BOOL_T;
}

void inner(void *data, int argc, char **argv)
{
    SCM data_pipe_fd = scm_from_int(((int*) data)[0]);
    SCM log_pipe_fd = scm_from_int(((int*) data)[1]);
    (void)argc;
    (void)argv;
    logs::debug << LOC() "Fixing load-path" << logs::done;
    scm_c_eval_string("(add-to-load-path \"./plugin/lisp/\")");
    logs::debug << LOC() "Defining send" << logs::done;
    scm_c_define_gsubr("send_ls", 1, 0, 0, (scm_t_subr) &send_ls);
    logs::debug << LOC() "Importing plugin" << logs::done;
    scm_c_use_module("plugin");
    logs::debug << LOC() "Looking up runcmd" << logs::done;
    SCM run_ls = scm_c_lookup("run_ls");
    logs::debug << LOC() "Running plugin" << logs::done;
    scm_call_2(run_ls, data_pipe_fd, log_pipe_fd);
}
