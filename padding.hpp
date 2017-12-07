//
// Created by huangqinjin on 4/24/17.
//

#ifndef HELLO_UTIL_PADDING_HPP
#define HELLO_UTIL_PADDING_HPP

#include <array>
#include <atomic>

namespace HELLO_NAMESPACE
{
    namespace util
    {
        template<std::size_t Bytes =
#ifndef NDEBUG
        512
#else
        0
#endif
        >
        struct padding
        {
            using pad_t = std::array<unsigned long,
                    (Bytes + sizeof(unsigned long) - 1) / sizeof(unsigned long)>;
            const pad_t pad;

            padding() : padding([] { static std::atomic<unsigned long> seq(-1);
                return seq.fetch_sub(1, std::memory_order_relaxed); }()) {}

            explicit padding(unsigned long x)
                : pad([x] { pad_t pad; for(auto&& i : pad) i = x; return pad; }()) {}

            padding(const padding&) = default;
            padding& operator=(const padding&) {}

            std::size_t check() const
            {
                const auto& x = pad[0];
                for(std::size_t i = 1; i < pad.size(); ++i)
                    if(x != pad[i]) return i;
                return 0;
            }
        };

        template<>
        struct padding<0>
        {
            using pad_t = std::array<unsigned long, 0>;
            explicit padding(unsigned long = 0) {}
            std::size_t check() const { return 0; }
        };
    }
}
#endif //HELLO_UTIL_PADDING_HPP
