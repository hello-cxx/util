//
// Created by huangqinjin on 4/25/17.
//

#include "../flags.hpp"

#include <mutex>

using namespace HELLO_NAMESPACE;
using namespace util;

namespace
{
    using integral = flags::integral;

    std::mutex guard;
    integral bits = static_cast<integral>(-1);

    integral allocate(std::size_t n)
    {
        integral mask{0};

        {
            std::lock_guard<std::mutex> lock(guard);
            integral tmp = bits;

            do
            {
                if(tmp == 0) return 0;
                integral t = tmp;
                tmp &= (tmp - 1);
                mask |= (t - tmp);
            } while (--n);

            bits = tmp;
        }

        return mask;
    }

    void deallocate(integral mask)
    {
        std::lock_guard<std::mutex> lock(guard);
        bits |= mask;
    }
}

flags::mask::mask(std::size_t bits)
        : value_{(bits >= 1 && bits <= flags::bits)
                 ? allocate(bits) : 0} {}

flags::mask::~mask()
{
    if(value_) deallocate(value_);
}

flags::mask::mask(mask&& other) : value_(other.value_)
{
    other.value_ = 0;
}

flags::mask& flags::mask::operator=(mask&& other)
{
    if(this != &other)
    {
        swap(other);
        mask(0).swap(other);
    }
    return *this;
}

void flags::mask::swap(mask& other)
{
    std::swap(value_, other.value_);
}

void flags::mask::swap(mask&& other)
{
    swap(other);
}

flags::mask& flags::mask::merge(mask& other)
{
    if(this != &other)
    {
        value_ |= other.value_;
        other.value_ = 0;
    }
    return *this;
}

flags::mask& flags::mask::merge(mask&& other)
{
    return merge(other);
}

flags::mask flags::mask::split()
{
    mask r(0);
    if(value_)
    {
        integral tmp = value_;
        value_ &= (value_ - 1);
        r.value_ = tmp - value_;
    }
    return r;
}

flags::mask flags::mask::split(integral bits)
{
    mask r(0);
    if((value_ & bits) == bits)
    {
        r.value_ = bits;
        value_ &= ~bits;
    }
    return r;
}

bool flags::mask::operator==(const mask& other) const
{
    return value_ == other.value_;
}

flags::mask::operator bool() const
{
    return value_ != 0;
}

integral flags::mask::value() const
{
    return value_;
}

flags::mask::operator integral() const
{
    return value();
}

flags::flags(integral initial) : flags_(initial) {}

flags::flags(const flags& other) : flags_(other.flags_.load(std::memory_order_relaxed)) {}

flags& flags::operator=(const flags& other)
{
    flags_.store(other.flags_.load(std::memory_order_relaxed), std::memory_order_relaxed);
    return *this;
}

integral flags::check(integral bits, std::memory_order order) const
{
    return flags_.load(order) & bits;
}

integral flags::set(integral bits)
{
    return flags_.fetch_or(bits, std::memory_order_acquire) & bits;
}

integral flags::reset(integral bits)
{
    return flags_.fetch_and(~bits, std::memory_order_release) & bits;
}

integral flags::checked_set(integral bits)
{
    integral value = flags_.load(std::memory_order_relaxed);
    while(((value & bits) == 0) && !flags_.compare_exchange_weak(value,
            value | bits, std::memory_order_acquire, std::memory_order_relaxed))
        continue;
    return value & bits;
}

integral flags::checked_reset(integral bits)
{
    integral value = flags_.load(std::memory_order_relaxed);
    while(((value & bits) == bits) && !flags_.compare_exchange_weak(value,
            value & ~bits, std::memory_order_release, std::memory_order_relaxed))
        continue;
    return value & bits;
}