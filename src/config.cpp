//
// Created by huangqinjin on 5/24/17.
//

#include "../config.hpp"
#include "../options.hpp"
#include "../threading.hpp"

using namespace HELLO_NAMESPACE;
using namespace util;

config::storage::storage(const cv::String& filename, int mode)
    : data(filename, mode), filename(filename) {}

bool config::storage::has(const char* key)
{
    return !data[key].empty();
}

cv::FileNode config::storage::get(const char* key, std::nothrow_t)
{
    return data[key];
}

cv::FileNode config::storage::get(const char* key)
{
    cv::FileNode node = data[key];
    if(node.empty())
    {
        if(this == &config::global())
        {
            throw std::out_of_range(
                    std::string("file [") + filename.c_str() +
                    "] has no value associated with key[" + key + ']');
        }
        else
        {
            throw std::out_of_range(
                    "thread [" + get_thread_name() +
                    "] has no value associated with key[" + key + ']');
        }
    }
    return node;
}

config::storage& config::global()
{
    static storage s(options::get<std::string>("param"),
                     cv::FileStorage::READ);
    return s;
}

config::storage& config::local()
{
    static thread_local storage s([](const cv::String& s) {
                                      return s.substr(s.rfind('.'));
                                  }(global().filename),
                                  cv::FileStorage::WRITE | cv::FileStorage::MEMORY);
    return s;
}

bool config::has(const char* key)
{
    return local().has(key) || global().has(key);
}

cv::FileNode config::get(const char* key, std::nothrow_t)
{
    cv::FileNode node = local().data[key];
    if(node.empty()) node = global().data[key];
    return node;
}

cv::FileNode config::get(const char* key)
{
    cv::FileNode node = local().data[key];
    if(node.empty())
    {
        node = global().data[key];
        if(node.empty())
        {
            throw std::out_of_range(
                    "thread [" + get_thread_name() +
                    "] and global[" + global().filename.c_str() +
                    "] has no value associated with key[" + key + ']');
        }
    }
    return node;
}

// Until C++20, requires at least one argument for the "..." in a variadic macro, see __VA_OPT__
SETUP_OPTIONS("Parameters Configuration",)
{
    options.add_options()
            ("param",
             boost::program_options::value<std::string>()
                     ->default_value("params.yaml"),
             "file of parameters [.yaml, .xml, .json]");
};