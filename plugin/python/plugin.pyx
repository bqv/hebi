import os

log = 0

cdef extern from "../python.hpp":
    void send_py(const char *line)
    void exit_py()

cdef public void run_py(int data_fd, int log_fd):
    global log
    log_debug("Working with FDs "+str(data_fd)+","+str(log_fd))
    log = log_fd
    while True:
        try:
            line = os.read(data_fd, 4096)
            handle(line.decode("utf-8", errors="ignore"))
        except BaseException as e:
            log_debug("Got exception "+str(e)+", exiting...")
            exit_py()

def handle(line):
    log_debug("In Python")
    if len(line) == 0:
        import sys
        sys.exit(-1)
    log_debug("PY-"+line)
    
def send(line):
    p_line = line.encode('utf-8')
    cdef char* c_line = p_line
    send_py(c_line)

def log_debug(line): 
    data = "+40 "+line
    os.write(log, data.encode("utf-8"))

def log_info(line):
    data = "+30 "+line
    os.write(log, data.encode("utf-8"))

def log_warn(line):
    data = "+20 "+line
    os.write(log, data.encode("utf-8"))

def log_error(line):
    data = "+10 "+line
    os.write(log, data.encode("utf-8"))

def log_fatal(line):
    data = "+00 "+line
    os.write(log, data.encode("utf-8"))
