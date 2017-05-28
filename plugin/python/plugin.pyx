cdef extern from "../python.hpp":
    void send_py(const char *line)

cdef public void handle_py(const char *line):
    p_line = line
    handle(p_line)

def send(line):
    cdef char* c_line = line
    send_py(c_line)

def handle(line):
    print("In Python")
    send("PRIVMSG ##doge :python")
