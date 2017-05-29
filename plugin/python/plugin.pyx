cdef extern from "../python.hpp":
    void send_py(const char *line)

cdef public void handle_py(const char *line):
    p_line = line
    handle(p_line.decode('utf-8'))

def send(line):
    p_line = line.encode('utf-8')
    cdef char* c_line = p_line
    send_py(c_line)

def handle(line):
    print("In Python")
    send("PRIVMSG ##doge :python")
