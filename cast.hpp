//
// Created by huangqinjin on 5/22/17.
//

#ifndef HELLO_UTIL_CAST_HPP
#define HELLO_UTIL_CAST_HPP

#include <type_traits>

namespace HELLO_NAMESPACE
{
    namespace util
    {
        template<class Base, class Derived>
        typename std::enable_if<std::is_base_of<
                typename std::remove_reference<Base>::type,
                typename std::remove_reference<Derived>::type>::value, Base>::type
            up_cast(Derived&& d) { return (Base)d; }

        template<class Base, class Derived>
        typename std::enable_if<
                std::is_pointer<Base>::value &&
                !std::is_pointer<Derived>::value &&
                std::is_base_of<
                typename std::remove_pointer<Base>::type,
                typename std::remove_reference<Derived>::type>::value, Base>::type
        up_cast(Derived* d) { return (Base)d; }
    }

    using util::up_cast;
}

#endif //HELLO_UTIL_CAST_HPP
