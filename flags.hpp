//
// Created by huangqinjin on 4/5/17.
//

#ifndef HELLO_UTIL_FLAGS_HPP
#define HELLO_UTIL_FLAGS_HPP

#include <cstdint>
#include <atomic>

namespace HELLO_NAMESPACE
{
    namespace util
    {
        class flags
        {
        public:
            using integral = std::uintmax_t;
            static const std::size_t bits = sizeof(integral) * 8;

            class mask
            {
            public:
                explicit mask(std::size_t bits = 1);
                mask(const mask&) = delete;
                mask& operator=(const mask&) = delete;
                mask(mask&& other);
                mask& operator=(mask&& other);
                ~mask();

                void swap(mask& other);
                void swap(mask&& other);
                mask& merge(mask& other);
                mask& merge(mask&& other);
                mask split();
                mask split(integral bits);

                bool operator==(const mask& other) const;
                explicit operator bool() const;
                operator integral() const;
                integral value() const;

            private:
                integral value_;
            };

        public:
            flags(integral initial = 0);
            flags(const flags& other);
            flags& operator=(const flags& other);

            integral check(integral bits, std::memory_order order = std::memory_order_seq_cst) const;
            integral set(integral bits);
            integral reset(integral bits);
            integral checked_set(integral bits);
            integral checked_reset(integral bits);

        private:
            std::atomic<integral> flags_;
        };
    }
}
#endif //HELLO_UTIL_FLAGS_HPP
