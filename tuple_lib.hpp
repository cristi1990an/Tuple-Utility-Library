#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <tuple>
#include <utility>
#include <type_traits>

namespace tuple_lib
{
    template<typename>
    struct common_tuple_type{};

    template<typename ... T>
    struct common_tuple_type<std::tuple<T...>>
    {
        using type = std::common_type_t<T...>;
    };

    template<typename ... T>
    struct common_tuple_type<std::pair<T...>>
    {
        using type = std::common_type_t<T...>;
    };

    template<typename T, std::size_t Size>
    struct common_tuple_type<std::array<T, Size>>
    {
        using type = T;
    };

    template<typename T>
    using common_tuple_type_t = typename common_tuple_type<T>::type;

    template <typename>
    struct is_tuple : std::false_type {};

    template <typename ... T>
    struct is_tuple<std::tuple<T...>> : std::true_type {};

    template<typename T>
    constexpr bool is_tuple_v = is_tuple<T>::value;

    template <typename>
    struct is_pair : std::false_type {};

    template <typename ... T>
    struct is_pair<std::pair<T...>> : std::true_type {};

    template<typename T>
    constexpr bool is_pair_v = is_pair<T>::value;

    template <typename>
    struct is_standard_array : std::false_type {};

    template <typename T, std::size_t Size>
    struct is_standard_array<std::array<T, Size>> : std::true_type {};

    template <typename T>
    constexpr bool is_standard_array_v = is_standard_array<T>::value;

    template<typename T>
    concept tuple_like = is_tuple_v<T> || is_pair_v<T> || is_standard_array_v<T>;

    namespace _impl_tuple_extension
    {
        template<typename T, typename Last>
        constexpr inline void _impl_emplace_back_all(std::vector<T>& container, Last&& last) noexcept
        {
            container.emplace_back(std::forward<Last>(last));
        }

        template<typename T, typename First, typename ... Args>
        constexpr inline void _impl_emplace_back_all(std::vector<T>& container, First&& first, Args&& ... args) noexcept
        {
            container.emplace_back(std::forward<First>(first));

            _impl_emplace_back_all(container, std::forward<Args>(args)...);
        }

        template<typename Tuple, std::size_t ... Indices>
        constexpr inline auto _impl_make_array_from_tuple(const Tuple& tuple, std::index_sequence<Indices...>)
        {
            using T = common_tuple_type_t<Tuple>;
            constexpr std::size_t Size = sizeof...(Indices);

            return std::array<T, Size> { static_cast<T>(std::get<Indices>(tuple)) ... };
        }
    }

    template<typename T, typename ... Args> requires (std::convertible_to<Args, T> && ...)
    constexpr inline void emplace_back_all(std::vector<T>& container, Args&& ... args) noexcept
    {
        container.reserve(container.size() + sizeof...(args));

        _impl_tuple_extension::_impl_emplace_back_all(container, std::forward<Args>(args)...);
    }

    template<typename T>
    constexpr inline void emplace_back_tuple(std::vector<T>& container, tuple_like auto&& tuple) noexcept
    {
        auto bind_container = [&container](auto&& ... args)
        {
            emplace_back_all(container, std::forward<decltype(args)>(args)...);
        };

        std::apply(bind_container, tuple);
    }

    constexpr inline auto make_vector(tuple_like auto& tuple) noexcept
    {
        using T = common_tuple_type_t<decltype(tuple)>;

        std::vector<T> result;

        emplace_back_tuple(result, tuple);

        return result;
    }

    template<typename ... Args>
    constexpr inline auto make_array(Args&& ... args) noexcept
    {
        using T = std::common_type_t<Args...>;
        constexpr std::size_t Size = sizeof...(Args);

        return std::array<T, Size>{ static_cast<T>(args)... };
    }

    constexpr inline auto make_array_from_tuple(const tuple_like auto& tuple) noexcept
    {
        constexpr std::size_t Size = std::tuple_size_v<std::remove_cvref_t<decltype(tuple)>>;

        return _impl_tuple_extension::_impl_make_array_from_tuple(tuple, std::make_index_sequence<Size>{});
    }
}