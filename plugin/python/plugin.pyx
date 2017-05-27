cdef extern from "../python.hpp":
    void send_py(const char *line)

cdef public void handle_py(const char *line):
    pass
