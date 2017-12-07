//
// Created by huangqinjin on 5/24/17.
//

#ifndef HELLO_UTIL_CONFIG_HPP
#define HELLO_UTIL_CONFIG_HPP

#include <opencv2/core/persistence.hpp>

namespace HELLO_NAMESPACE
{
    namespace util
    {
        class config
        {
        public:
            class storage
            {
                friend class config;

                cv::FileStorage data;
                cv::String filename;

                storage(const cv::String& filename, int mode);

            public:
                bool has(const char* key);
                cv::FileNode get(const char* key);
                cv::FileNode get(const char* key, std::nothrow_t);

                template<typename T>
                T get(const char* key)
                {
                    return T(get(key));
                }

                template<typename T>
                bool get(const char* key, T& value)
                {
                    auto node = get(key, std::nothrow);
                    if(node.empty()) return false;
                    value = T(node);
                    return true;
                }
            };

            static storage& global();
            static storage& local();

            static bool has(const char* key);
            static cv::FileNode get(const char* key);
            static cv::FileNode get(const char* key, std::nothrow_t);

            template<typename T>
            static T get(const char* key)
            {
                return T(get(key));
            }

            template<typename T>
            static bool get(const char* key, T& value)
            {
                auto node = get(key, std::nothrow);
                if(node.empty()) return false;
                value = T(node);
                return true;
            }

            template<typename T>
            static void set(const char* key, const T& value)
            {
                local().data << key << value;
            }
        };

    }
}
#endif //HELLO_UTIL_CONFIG_HPP
