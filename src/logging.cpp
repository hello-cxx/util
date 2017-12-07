//
// Created by huangqinjin on 4/27/17.
//

#include "../logging.hpp"
#include "../options.hpp"
#include "../threading.hpp"

#include <boost/log/expressions.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/function.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/support/date_time.hpp>

using namespace HELLO_NAMESPACE;
using namespace util;

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

static_assert(BOOST_PHOENIX_VERSION >= 0x3000,"BOOST_PHOENIX_VERSION error!");

namespace boost
{
    BOOST_LOG_OPEN_NAMESPACE
    namespace trivial
    {
        std::ostream& operator<<(std::ostream& strm, severity_level lvl)
        {

            static const char str[6][6] =
            {
                {'T','R','A','C','E',0},
                {'D','E','B','E','G',0},
                {'I','N','F','O',0},
                {'W','A','R','N',0},
                {'E','R','R','O','R',0},
                {'F','A','T','A','L',0}
            };

            if (static_cast<std::size_t>(lvl) < 6)
                strm << str[lvl];
            else
                strm << static_cast<int>(lvl);

            return strm;
        }
    }
    BOOST_LOG_CLOSE_NAMESPACE
}


namespace HELLO_NAMESPACE
{
    namespace util
    {
        std::ostream& operator<<(std::ostream& strm, const logger::location& loc)
        {
            return strm << loc.file << ':' << loc.line;
        }
    }
}

namespace
{
    BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", attrs::local_clock::value_type)
    BOOST_LOG_ATTRIBUTE_KEYWORD(uptime, "Uptime", attrs::timer::value_type)
    BOOST_LOG_ATTRIBUTE_KEYWORD(scope, "Scope", attrs::named_scope::value_type)
    BOOST_LOG_ATTRIBUTE_KEYWORD(location, "Location", logger::location)
    BOOST_LOG_ATTRIBUTE_KEYWORD(thread, "Thread", std::string)
    BOOST_LOG_ATTRIBUTE_KEYWORD(file, "File", logging::string_literal)
    BOOST_LOG_ATTRIBUTE_KEYWORD(line, "Line", int)

    // Custom formatter factory to add Scope format support in config ini file.
    // Allows %Scope(format="%n %c %F:%l")% to be used in ini config file for property Format.
    class namedscope_formatter_factory :
            public logging::basic_formatter_factory<char, attrs::named_scope::value_type>
    {
        formatter_type create_formatter(logging::attribute_name const& name, args_map const& args)
        {
            auto iter = args.find("format");
            if (iter == args.end()) return expr::stream << scope;
            return expr::stream << expr::format_named_scope(scope, keywords::format = iter->second);
        }
    };

    // Custom formatter factory to add TimeStamp format support in config ini file.
    // Allows %TimeStamp(format="%Y.%m.%d %H:%M:%S.%f")% to be used in ini config file for property Format.
    class timestamp_formatter_factory :
            public logging::basic_formatter_factory<char, boost::posix_time::ptime>
    {
        formatter_type create_formatter(const boost::log::attribute_name& name, const args_map& args)
        {
            auto iter = args.find("format");
            if (iter == args.end()) return expr::stream << timestamp;
            return expr::stream << expr::format_date_time(timestamp, iter->second);
        }
    };

    // Custom formatter factory to add Uptime format support in config ini file.
    // Allows %Uptime(format="%O:%M:%S.%f")% to be used in ini config file for property Format.
    // boost::log::attributes::timer value type is boost::posix_time::time_duration
    class uptime_formatter_factory :
            public boost::log::basic_formatter_factory<char, boost::posix_time::time_duration>
    {
        formatter_type create_formatter(const boost::log::attribute_name& name, const args_map& args)
        {
            auto iter = args.find("format");
            if (iter == args.end()) return expr::stream << uptime;
            return expr::stream << expr::format_date_time(uptime, iter->second);
        }
    };
}

void logger::init(const std::string& file)
{
    std::cout << "Configure logger: " << file << std::endl;

    logging::register_formatter_factory(::scope.get_name(), boost::make_shared<namedscope_formatter_factory>());
    logging::register_formatter_factory(::timestamp.get_name(), boost::make_shared<timestamp_formatter_factory>());
    logging::register_formatter_factory(::uptime.get_name(), boost::make_shared<uptime_formatter_factory>());
    logging::register_simple_formatter_factory<logging::trivial::severity_level, char>(logging::trivial::severity.get_name());
    logging::register_simple_formatter_factory<location, char>(::location.get_name());

    //logging::add_common_attributes();
    auto core = logging::core::get();
    core->add_global_attribute(::timestamp.get_name(), attrs::local_clock());
    core->add_global_attribute(::uptime.get_name(), attrs::timer());
    core->add_global_attribute(::scope.get_name(), attrs::named_scope());
    core->add_global_attribute(::thread.get_name(), attrs::make_function((std::string(*)())&get_thread_name));

    logging::formatter fmt = expr::stream
            << expr::format_date_time(::timestamp, "%H:%M:%S.%f") << ' '
            << ::thread << ' '
            << logging::trivial::severity << ' '
            << ::location << ' '
            << expr::message;


    auto sink = logging::add_console_log(std::clog);
    sink->set_formatter(fmt);

}

// Until C++20, requires at least one argument for the "..." in a variadic macro, see __VA_OPT__
SETUP_OPTIONS("Logger Configuration",)
{
    options.add_options()
            ("log",
             boost::program_options::value<std::string>()
                     ->default_value("")
                     ->notifier(&logger::init),
             "file of log configuration");
};