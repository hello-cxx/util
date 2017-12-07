//
// Created by huangqinjin on 4/29/17.
//

#include "../threading.hpp"

#include <sstream>
#include <unordered_map>
#include <mutex>

//#include <sys/prctl.h>  // for prctl
#include <pthread.h>    // for pthread_setname_np

using namespace HELLO_NAMESPACE;
using namespace util;

namespace
{
    struct thread_name
    {
        static std::mutex name_map_guard;
        static std::unordered_map<std::thread::id, std::string> name_map;
        static thread_local thread_name name;

        std::unordered_map<std::thread::id, std::string>::iterator iter;
        thread_name()
        {
            auto id = std::this_thread::get_id();
            std::stringstream ss;
            ss << id;
            {
                std::lock_guard<std::mutex> lock(name_map_guard);
                iter = name_map.emplace(id, ss.str()).first;
            }
        }

        ~thread_name()
        {
            std::lock_guard<std::mutex> lock(name_map_guard);
            name_map.erase(iter);
        }

        static void set(const std::string& nm)
        {
            //prctl(PR_SET_NAME, nm.c_str(), 0, 0, 0);
            pthread_setname_np(pthread_self(), nm.c_str());
            name.iter->second = nm;
        }

        static void set(std::thread& thread, const std::string& nm)
        {
            pthread_setname_np(thread.native_handle(), nm.c_str());
            std::lock_guard<std::mutex> lock(name_map_guard);
            auto iter = name_map.emplace(thread.get_id(), nm);
            if(!iter.second) iter.first->second = nm;
        }

        static std::string get()
        {
            return name.iter->second;
        }

        static std::string get(std::thread& thread)
        {
            auto id = std::this_thread::get_id();
            {
                std::lock_guard<std::mutex> lock(name_map_guard);
                auto iter = name_map.find(thread.get_id());
                if(iter != name_map.end()) return iter->second;
            }

            std::stringstream ss;
            ss << id;
            return ss.str();
        }
    };

    std::mutex thread_name::name_map_guard;
    std::unordered_map<std::thread::id, std::string> thread_name::name_map;
    thread_local thread_name thread_name::name;
}

namespace HELLO_NAMESPACE
{
    namespace util
    {
        void set_thread_name(const std::string& name)
        {
            return thread_name::set(name);
        }

        void set_thread_name(std::thread& thread, const std::string& name)
        {
            return thread_name::set(thread, name);
        }

        std::string get_thread_name()
        {
            return thread_name::get();
        }

        std::string get_thread_name(std::thread& thread)
        {
            return thread_name::get(thread);
        }
    }
}