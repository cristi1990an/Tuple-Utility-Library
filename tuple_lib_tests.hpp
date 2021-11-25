#pragma once

#include <array>
#include <vector>
#include <tuple>
#include <utility>
#include <type_traits>
#include <cassert>
#include <algorithm>
#include "tuple_lib.hpp"

namespace tuple_lib_tests
{
    constexpr void run_tests() noexcept
    {
        static_assert(tuple_lib::tuple_like<std::tuple<int, float, double>>);
        static_assert(tuple_lib::tuple_like<std::pair<int, float>>);
        static_assert(tuple_lib::tuple_like<std::array<double, 10>>);

        auto expected = { (double)1, (double)2, (double)3.3f, (double)4.4 };

        auto array = tuple_lib::make_array(1, (char)2, 3.3f, 4.4);

        static_assert(std::is_same_v<decltype(array), std::array<double, 4>>);

        assert(std::equal(array.cbegin(), array.cend(), expected.begin(), expected.end()));

        auto tuple = std::make_tuple(1, (char)2, 3.3f, 4.4);

        auto array_from_tuple = tuple_lib::make_array_from_tuple(tuple);

        static_assert(std::is_same_v<decltype(array), decltype(array_from_tuple)>);

        assert(std::equal(array_from_tuple.cbegin(), array_from_tuple.cend(), expected.begin(), expected.end()));

        auto pair = std::make_pair(5, 5.5f);

        auto array_from_pair = tuple_lib::make_array_from_tuple(pair);

        static_assert(std::is_same_v<decltype(array_from_pair), std::array<float, 2>>);

        assert(array_from_pair[0] == pair.first);
        assert(array_from_pair[1] == pair.second);

        auto array_from_array = tuple_lib::make_array_from_tuple(array);

        static_assert(std::is_same_v<decltype(array_from_array), decltype(array)>);

        assert(std::equal(array_from_array.cbegin(), array_from_array.cend(), expected.begin(), expected.end()));
    }
}