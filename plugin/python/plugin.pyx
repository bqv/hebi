import os

cdef extern from "../python.hpp":
    void send_py(const char *line)
    void log_debug_py(const char *line)
    void log_info_py(const char *line)
    void log_warn_py(const char *line)
    void log_error_py(const char *line)
    void log_fatal_py(const char *line)

cdef public void run_py(int fd):
    while True:
        log_debug("Reading file")
        string = ""
        string = os.read(fd, 4096).decode("utf-8", errors="ignore")
        log_debug("Calling handler")
        handle(string[:-1])
    
cdef public void handle_py(const char *line):
    log_debug("In Python")
    p_line = line
    handle(p_line.decode('utf-8'))

def send(line):
    p_line = line.encode('utf-8')
    cdef char* c_line = p_line
    send_py(c_line)

def log_debug(line):
    p_line = line.encode('utf-8')
    cdef char* c_line = p_line
    log_debug_py(c_line)

def log_info(line):
    p_line = line.encode('utf-8')
    cdef char* c_line = p_line
    log_info_py(c_line)

def log_warn(line):
    p_line = line.encode('utf-8')
    cdef char* c_line = p_line
    log_warn_py(c_line)

def log_error(line):
    p_line = line.encode('utf-8')
    cdef char* c_line = p_line
    log_error_py(c_line)

def log_fatal(line):
    p_line = line.encode('utf-8')
    cdef char* c_line = p_line
    log_fatal_py(c_line)

def handle(line):
    log_debug("PY-"+line)
