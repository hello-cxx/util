//
// Created by huangqinjin on 4/25/17.
//

#include "../flags.hpp"

#include <boost/test/unit_test.hpp>
#include <iostream>

using namespace HELLO_NAMESPACE;
using namespace util;

BOOST_AUTO_TEST_SUITE(util_flags)

BOOST_AUTO_TEST_CASE(mask_alloc)
{
    {
        flags::mask masks[flags::bits];

        for(const auto& m : masks)
        {
            BOOST_TEST(m.value() != 0);
        }

        flags::mask invalid;
        BOOST_TEST(!invalid);
    }

    {
        flags::mask masks[flags::bits];

        for(const auto& m : masks)
        {
            BOOST_TEST(m.value() != 0);
        }

        flags::mask invalid;
        BOOST_TEST(!invalid);
    }
}

BOOST_AUTO_TEST_CASE(set_flag)
{
    flags::mask masks[3];
    flags flags;
    BOOST_TEST(flags.check(masks[0] | masks[1] | masks[2]) == 0);

    BOOST_TEST(flags.set(masks[0] | masks[1]) == 0);
    BOOST_TEST(flags.set(masks[1]) == masks[1]);
    BOOST_TEST(flags.reset(masks[0]) == masks[0]);
    BOOST_TEST(flags.reset(masks[2]) == 0);
    BOOST_TEST(flags.check(masks[0] | masks[2]) == 0);

    BOOST_TEST(flags.checked_set(masks[1]) == masks[1]);
    BOOST_TEST(flags.checked_set(masks[2]) == 0);
    BOOST_TEST(flags.check(masks[2]) == masks[2]);
    BOOST_TEST(flags.checked_reset(masks[0]) == 0);
    BOOST_TEST(flags.checked_reset(masks[1]) == masks[1]);
    BOOST_TEST(flags.check(masks[0] | masks[1] | masks[2]) == masks[2]);
}

BOOST_AUTO_TEST_SUITE_END()