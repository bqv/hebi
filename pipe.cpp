#include "pipe.hpp"

namespace pipes
{
    pipe::pipe()
    {
        mPipefd = 0;
    }
    
    pipe::pipe(int pPipefd)
    {
        mPipefd = pPipefd;
    }

    pipe::~pipe()
    {
    }

    int pipe::fd()
    {
        return mPipefd;
    }

    std::string pipe::read()
    {
        char buffer[4096];
        ssize_t len = ::read(mPipefd, buffer, 4096);
        if (len < 0)
        {
            logs::fatal << LOC() << "(" << mPipefd << ") Error reading from pipe [" << std::strerror(errno) << "]" << logs::done;
            exit(-1);
        }
        else
        {
            std::string data(buffer, len);
            if (data.length() > 0 && data[0] == '+')
            {
                switch(data[1])
                {
                  case '0':
                    logs::fatal << data.substr(4) << logs::done;
                  case '1':
                    logs::error << data.substr(4) << logs::done;
                  case '2':
                    logs::warn << data.substr(4) << logs::done;
                  case '3':
                    logs::info << data.substr(4) << logs::done;
                  case '4':
                    logs::debug << data.substr(4) << logs::done;
                }
                return std::string();
            }
            else
            {
                return data;
            }
        }
    }

    void pipe::write(std::string pData)
    {
        ::write(mPipefd, pData.c_str(), pData.length());
    }
}
