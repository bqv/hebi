import Handler

cdef extern from "../python.hpp":
    void send_py(const char *line)

def send(line):
    cdef char* c_line = line
    send_py(c_line)

h = Handler(send)

cdef public void handle_py(const char *line):
    p_line = line
    h.handle(p_line)
