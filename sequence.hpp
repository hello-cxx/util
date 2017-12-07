//
// Created by huangqinjin on 4/5/17.
//

#ifndef HELLO_UTIL_SEQUENCE_HPP
#define HELLO_UTIL_SEQUENCE_HPP

#include <atomic>

namespace HELLO_NAMESPACE
{
    namespace util
    {
        template<typename Tag>
        class sequence
        {
        public:
            using tag = Tag;
            using type = unsigned long;

            sequence() : sequence([] { static std::atomic<type> seq(1);
                return seq.fetch_add(1, std::memory_order_relaxed); }()) {}
            explicit sequence(type id) : id_(id) {}
            sequence(const sequence&) = default;
            sequence& operator=(const sequence&) { return *this; }

            type id() const
            {
                return id_;
            }

            bool operator<(const sequence& other) const
            {
                return id_ < other.id_;
            }

            bool operator==(const sequence& other) const
            {
                return id_ == other.id_;
            }

        private:
            const type id_;
        };

        template<typename Tag>
        void reassign(sequence<Tag>& seq, typename sequence<Tag>::type id)
        {
            seq.~sequence();
            new ((void*)&seq) sequence<Tag>(id);
        }
    }
}

namespace std
{
    template<typename Key>
    struct hash;

    template<typename Tag>
    struct hash<::HELLO_NAMESPACE::util::sequence<Tag>>
    {
        using argument_type = ::HELLO_NAMESPACE::util::sequence<Tag>;
        using result_type = size_t;
        size_t operator()(const argument_type& k) const
        {
            return hash<typename argument_type::type>{}(k.id());
        }
    };
}
#endif //HELLO_UTIL_SEQUENCE_HPP
