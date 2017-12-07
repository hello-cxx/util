//
// Created by huangqinjin on 11/30/17.
//

#ifndef HELLO_UTIL_DYNSPAN_HPP
#define HELLO_UTIL_DYNSPAN_HPP

#include <new>       // for placement new
#include <stdexcept> // for std::out_of_range
#include <iterator>  // for std::random_access_iterator_tag, std::reverse_iterator
#include <cstddef>   // for std::size_t, std::ptrdiff_t
#include <cstdint>   // for std::intptr_t, std::uintptr_t

namespace HELLO_NAMESPACE
{
    namespace util
    {
        template<typename T>
        class dynspan
        {
            T* p;
            std::size_t n;
            std::size_t s;

        public:
            class iterator
            {
                T* p;
                std::size_t s;

            public:
                using difference_type = std::ptrdiff_t;
                using value_type = T;
                using pointer = T*;
                using reference = T&;
                using iterator_category	= std::random_access_iterator_tag;

                explicit iterator(T* p = nullptr, std::size_t s = sizeof(T)) : p(p), s(s) {}

                template<typename U>
                iterator(const typename dynspan<U>::iterator& other)
                        : p(other.p), s(other.s) {}

                iterator& operator+=(difference_type n)
                {
                    auto a = reinterpret_cast<std::intptr_t>(p);
                    a += s * n;
                    p = reinterpret_cast<T*>(a);
                    return *this;
                }

                difference_type operator-(const iterator& other) const
                {
                    auto a = reinterpret_cast<std::intptr_t>(p);
                    auto b = reinterpret_cast<std::intptr_t>(other.p);
                    return reinterpret_cast<difference_type>((a - b) / s);
                }

                pointer operator->() const { return p; }
                reference operator*() const { return *p; }
                reference operator[](difference_type n) const { return *((*this) + n); }
                iterator& operator-=(difference_type n) { return (*this) += (-n); }
                iterator operator+(difference_type n) const { auto tmp = *this; return tmp += n; }
                iterator operator-(difference_type n) const { return (*this) + (-n); }
                iterator& operator++() { return (*this) += 1; }
                iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }
                iterator& operator--() { return (*this) -= 1; }
                iterator operator--(int) { auto tmp = *this; --(*this); return tmp; }
                bool operator==(const iterator& other) const { return p == other.p; }
                bool operator!=(const iterator& other) const { return p != other.p; }
                bool operator<(const iterator& other) const { return p < other.p; }
                bool operator>(const iterator& other) const { return p > other.p; }
                bool operator<=(const iterator& other) const { return p <= other.p; }
                bool operator>=(const iterator& other) const { return p >= other.p; }
            };

            using const_iterator = typename dynspan<const T>::iterator;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        public:
            dynspan() : p(nullptr), n(0), s(0) {}

            template<typename U>
            dynspan(U* p, std::size_t n, std::size_t s = sizeof(U))
                    : p(p), n(n), s(s) {}

            template<typename U, std::size_t N>
            dynspan(U (&arr) [N]) : dynspan(arr, N) {}

            template<typename Container>
            dynspan(Container& c) : dynspan(c.data(), c.size()) {}

            T* ptr(std::size_t i)
            {
                auto a = reinterpret_cast<std::uintptr_t>(p);
                a += s * i;
                return reinterpret_cast<T*>(a);
            }

            const T* ptr(std::size_t i) const
            {
                auto a = reinterpret_cast<std::uintptr_t>(p);
                a += s * i;
                return reinterpret_cast<T*>(a);
            }

            void construct()
            {
                // C++17 Algorithm
                auto uninitialized_value_construct = [](iterator first, iterator last)
                {
                    iterator current = first;
                    try
                    {
                        for (; current != last; ++current)
                            ::new ((void*)current.operator->()) T{};
                    }
                    catch (...)
                    {
                        for (; first != current; ++first)
                            first->~T();
                        throw;
                    }
                };

                uninitialized_value_construct(begin(), end());
            }

            void destroy()
            {
                // C++17 Algorithm
                auto destroy = [](iterator first, iterator last)
                {
                    for (; first != last; ++first)
                        first->~T();
                };

                destroy(begin(), end());
            }

            template<typename U>
            dynspan<U> cast() const
            {
                return { (U*)p, n, s };
            }

            T& at(std::size_t i)
            {
                if(i >= n) throw std::out_of_range("dynspan::at out of range");
                return (*this)[i];
            }

            const T& at(std::size_t i) const
            {
                if(i >= n) throw std::out_of_range("dynspan::at out of range");
                return (*this)[i];
            }

            std::size_t step() const { return s; }
            std::size_t size() const { return n; }
            bool empty() const { return p == nullptr; }
            T* data() { return ptr(0); }
            const T* data() const { return ptr(0); }
            T& operator[](std::size_t i) { return *ptr(i); }
            const T& operator[](std::size_t i) const { return *ptr(i); }
            iterator begin() { return iterator(ptr(0), s); }
            iterator end() { return iterator(ptr(n), s); }
            const_iterator begin() const { return const_iterator(ptr(0), s); }
            const_iterator end() const { return const_iterator(ptr(n), s); }
            const_iterator cbegin() const { return begin(); }
            const_iterator cend() const { return end(); }
            reverse_iterator rbegin() { return reverse_iterator(end()); }
            reverse_iterator rend() { return reverse_iterator(begin()); }
            const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
            const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
            const_reverse_iterator crbegin() const { return rbegin(); }
            const_reverse_iterator crend() const { return rend(); }
        };
    }
}
#endif //HELLO_UTIL_DYNSPAN_HPP
