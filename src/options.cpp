//
// Created by huangqinjin on 5/24/17.
//

#include "../options.hpp"

#include <boost/program_options.hpp>

#include <iostream>

using namespace HELLO_NAMESPACE;
using namespace util;
using namespace boost::program_options;

namespace
{
    const char* const version = "SimpleSLAM 1.0.0.0";

    struct data_holder
    {
        options_description all;
        variables_map vm;
    };

    data_holder& data()
    {
        static data_holder d;
        return d;
    }
}

void options::add(boost::program_options::options_description& desc)
{
    data().all.add(desc);
}

void options::parse(int argc, char* argv[])
{
    auto& d = data();
    d.all.add_options()
            ("version,v", "print version string")
            ("help,h", "produce help message");

    store(command_line_parser(argc, argv).options(d.all).run(), d.vm);

    if (d.vm.count("help"))
    {
        std::cout << d.all << std::endl;
        std::exit(0);
    }

    if(d.vm.count("version"))
    {
        std::cout << version << std::endl;
        std::exit(0);
    }

    notify(d.vm);
}

bool options::has(const char* key)
{
    return data().vm.count(key) > 0;
}

const boost::program_options::variable_value& options::get(const char* key)
{
    return data().vm[key];
}