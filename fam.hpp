//
// Created by huangqinjin on 11/30/17.
//

#ifndef HELLO_UTIL_FAM_HPP
#define HELLO_UTIL_FAM_HPP

#define FAM_USE_ALIGNED_NEW 0
#define FAM_USE_ALIGNED_ALLOC 0
#define FAM_USE_NEW 1
#define FAM_USE_MALLOC 1

#include <new>
#include <memory> // for std::align, std::unique_ptr, std::default_delete

#if !FAM_USE_ALIGNED_NEW && FAM_USE_ALIGNED_ALLOC
#include <cstdlib> // for std::aligned_alloc
#endif

namespace HELLO_NAMESPACE
{
    namespace util
    {
        // emulation of Flexible Array Member introduced in C99
        template<typename H, typename A>
        struct fam
        {
            fam() = delete;
            ~fam() = delete;
            fam(const fam&) = delete;
            fam& operator=(const fam&) = delete;

            H h;
            A a[1];

#if FAM_USE_ALIGNED_NEW
            static fam* allocate(std::size_t n)
            {
                const std::size_t align = alignof(fam);
                const std::size_t size = sizeof(fam) + sizeof(A) * (n - 1);
                return (fam*)::operator new(size, std::align_val_t{align});
            }

            static void deallocate(fam* p, std::size_t n)
            {
                (void)n;
                const std::size_t align = alignof(fam);
                ::operator delete((void*)p, std::align_val_t{align});
            }
#elif FAM_USE_ALIGNED_ALLOC
            static fam* allocate(std::size_t n)
            {
                const std::size_t align = alignof(fam);
                const std::size_t size = sizeof(fam) + sizeof(A) * (n - 1);
                // The size parameter must be an integral multiple of alignment.
                auto p = (fam*)std::aligned_alloc(align, (size + align - 1) / align);
                if(p == nullptr) throw std::bad_alloc();
                return p;
            }

            static void deallocate(fam* p, std::size_t n)
            {
                (void)n;
                std::free((void*)p);
            }
#else
            static fam* allocate(std::size_t n)
            {
                const std::size_t align = alignof(fam);
                const std::size_t size = sizeof(fam) + sizeof(A) * (n - 1);
                std::size_t space = align + size;
#if FAM_USE_NEW
                auto p = (unsigned char*)::operator new(space, std::nothrow);
#else
                auto p = (unsigned char*)std::malloc(space);
#endif
                void* q = p + 1;
                if(!(p && std::align(align, size, q, --space)))
                {
#if FAM_USE_NEW
                    ::operator delete((void*)p);
#else
                    std::free((void*)p);
#endif
                    throw std::bad_alloc();
                }

                // std::cout << "allocate " << (void*)p << "  " << (void*)q << std::endl;

                p = static_cast<unsigned char*>(q) - 1;
                space = align + size - space; // bytes used for alignment
                if(space == 1)
                {
                    *p = UCHAR_MAX;
                }
                else
                {
                    assert(space < UCHAR_MAX * UCHAR_MAX + UCHAR_MAX);
                    *p = static_cast<unsigned char>(space % UCHAR_MAX);
                    *(p - 1) = static_cast<unsigned char>(space / UCHAR_MAX);
                }
                return (fam*)q;

            }

            static void deallocate(fam* p, std::size_t n)
            {
                (void)n;
                auto q = reinterpret_cast<unsigned char*>(p) - 1;
                if(*q != UCHAR_MAX) q -= *q + *(q - 1) * UCHAR_MAX - 1;

                // std::cout << "deallocate " << (void*)q << "  " << (void*)p << std::endl;

#if FAM_USE_NEW
                ::operator delete((void*)q);
#else
                std::free((void*)q);
#endif
            }
#endif

            template<typename ...Args>
            static fam* construct(std::size_t n, Args&&... args)
            {
                assert(n >= 1);
                auto p = allocate(n);
                int i = -1;
                try
                {
                    ::new ((void*)p) H{std::forward<Args>(args)...};
                    for(i = 0; i < (int)n; ++i) ::new ((void*)(p->a + i)) A{};
                }
                catch(...)
                {
                    if(i >= 0)
                    {
                        while(i--) (p->a + i)->~A();
                        p->h.~H();
                    }
                    deallocate(p, n);
                    throw;
                }
                return p;
            }

            void destroy(std::size_t n)
            {
                fam* p = this;
                // C++17 Algorithm
                auto destroy = [](A* first, A* last)
                {
                    for (; first != last; ++first)
                        first->~A();
                };

                destroy(p->a + 0, p->a + n);
                p->h.~H();
                deallocate(p, n);
            }

            template<typename ...Args>
            static std::unique_ptr<fam> create(std::size_t n, Args&&... args)
            {
                return std::unique_ptr<fam>(construct(n, std::forward<Args>(args)...),
                                            std::default_delete<fam>(n));
            }
        };
    }
}

namespace std
{
    template<typename H, typename A>
    struct default_delete<::HELLO_NAMESPACE::util::fam<H, A>>
    {
        size_t n;
        explicit default_delete(size_t n = 0) : n(n) {}
        void operator()(::HELLO_NAMESPACE::util::fam<H, A>* p) const { p->destroy(n); }
    };
}

#endif //HELLO_UTIL_FAM_HPP
