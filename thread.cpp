#include "thread.hpp"

namespace thread
{
    std::unordered_map<std::thread::id, std::string> names;
    std::mutex mutex;

    std::string get_current_name()
    {
        std::thread::id tid = std::this_thread::get_id();
        std::lock_guard<std::mutex> lock(mutex);
        return names[tid];
    }
}
