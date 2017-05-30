#ifndef _PIPE_HPP_
#define _PIPE_HPP_

#include <string>
#include <unistd.h>
#include "logger.hpp"

namespace pipes
{
    class pipe
    {
      private:
        int mPipefd;

      public:
        pipe();
        pipe(int pPipefd);
        ~pipe();

        int fd();
        std::string read();
        void write(std::string pData);
    };
}

#endif /*PIPE_HPP*/
