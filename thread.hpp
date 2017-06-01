#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <unordered_map>
#include <thread>
#include <mutex>

namespace thread
{
    extern std::unordered_map<std::thread::id, std::string> names;
    extern std::mutex mutex;

    template<typename... Types>
    std::thread make_thread(std::string pName, Types... pArgs);
    template<typename... Types>
    std::thread* make_thread_ptr(std::string pName, Types... pArgs);

    std::string get_current_name();
    void rename_current(std::string pName);
    void init();
}

#include "logger.hpp"

#include "thread.ipp"

#endif /*THREAD_HPP*/
