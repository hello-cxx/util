//
// Created by huangqinjin on 5/24/17.
//

#ifndef HELLO_UTIL_OPTIONS_HPP
#define HELLO_UTIL_OPTIONS_HPP

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

#define HELLO_UTIL_CAT_IMPL(a, b) a##b
#define HELLO_UTIL_CAT(a, b) HELLO_UTIL_CAT_IMPL(a, b)
#define HELLO_UTIL_UNIQUE_IDENTIFIER(a) HELLO_UTIL_CAT(a, __LINE__)

#define HELLO_UTIL_SETUP(a, ...) \
    static const struct a { template<typename F> a(F&& f) { f(); } } a = [__VA_ARGS__]

#define SETUP(...) HELLO_UTIL_SETUP(HELLO_UTIL_UNIQUE_IDENTIFIER(setup), __VA_ARGS__)

namespace HELLO_NAMESPACE
{
    namespace util
    {
        class options
        {
        public:
            static void add(boost::program_options::options_description&);
            static void parse(int argc, char* argv[]);

            static bool has(const char* key);
            static const boost::program_options::variable_value& get(const char* key);

            template<typename T>
            static const T& get(const char* key)
            {
                return get(key).as<T>();
            }
        };
    }
}

#define SETUP_OPTIONS_IMPL(a, g, ...)                                           \
    static const struct a { template<typename F> a(F&& f) {                     \
        ::boost::program_options::options_description options(g);               \
        f(options); ::HELLO_NAMESPACE::util::options::add(options); } } a =     \
        [__VA_ARGS__](::boost::program_options::options_description& options)

#define SETUP_OPTIONS(...) SETUP_OPTIONS_IMPL(HELLO_UTIL_UNIQUE_IDENTIFIER(setup), __VA_ARGS__)

#endif //HELLO_UTIL_OPTIONS_HPP
