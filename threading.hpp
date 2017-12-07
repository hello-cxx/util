//
// Created by huangqinjin on 4/29/17.
//

#ifndef HELLO_UTIL_THREADING_HPP
#define HELLO_UTIL_THREADING_HPP


#include <string>
#include <thread>

namespace HELLO_NAMESPACE
{
    namespace util
    {
        void set_thread_name(const std::string& name);
        void set_thread_name(std::thread& thread, const std::string& name);
        std::string get_thread_name();
        std::string get_thread_name(std::thread& thread);
    }
}

namespace std
{
    namespace this_thread
    {
        inline std::string get_name()
        {
            return ::HELLO_NAMESPACE::util::get_thread_name();
        }

        inline void set_name(const std::string& name)
        {
            return ::HELLO_NAMESPACE::util::set_thread_name(name);
        }
    }
}

#endif //HELLO_UTIL_THREADING_HPP
