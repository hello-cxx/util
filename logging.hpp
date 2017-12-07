//
// Created by huangqinjin on 4/27/17.
//

#ifndef HELLO_UTIL_LOGGING_HPP
#define HELLO_UTIL_LOGGING_HPP

#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

namespace HELLO_NAMESPACE
{
    namespace util
    {
        class null_logger
        {
        public:
            template<typename T>
            null_logger& operator<<(const T&) { return *this; }
        };

        class logger : public boost::log::trivial::logger_type
        {
        public:
            struct location
            {
                const char* file;
                unsigned int line;
            };

            static void init(const std::string& file);
        };
    }
}

#define NULL_LOG(...) while(false) ::HELLO_NAMESPACE::util::null_logger{}
#define BOOST_LOG_LOC_TRIVIAL(lvl) BOOST_LOG_TRIVIAL(lvl) << ::boost::log::add_value("Location", ::HELLO_NAMESPACE::util::logger::location{__FILE__, __LINE__})
#define LOG_TRACE BOOST_LOG_LOC_TRIVIAL(trace)
#define LOG_DEBUG BOOST_LOG_LOC_TRIVIAL(debug)
#define LOG_INFO BOOST_LOG_LOC_TRIVIAL(info)
#define LOG_WARN BOOST_LOG_LOC_TRIVIAL(warning)
#define LOG_ERROR BOOST_LOG_LOC_TRIVIAL(error)
#define LOG_FATAL BOOST_LOG_LOC_TRIVIAL(fatal)
#define LOG(lvl) LOG_ ## lvl

#endif //HELLO_UTIL_LOGGING_HPP
