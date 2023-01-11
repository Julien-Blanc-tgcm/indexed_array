// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_UNION_OF_H
#define JBC_INDEXED_ARRAY_DETAIL_UNION_OF_H

#include "default_indexer.hpp"

namespace jbc::indexed_array::detail
{

template <auto T1, auto T2, typename T = void>
struct diff_of
{
};

template <auto T1, auto T2>
struct diff_of<T1,
               T2,
               std::enable_if_t<std::is_same_v<decltype(T1), decltype(T2)> && std::is_enum_v<decltype(T1)>, void> >
{
	static constexpr auto value =
	    static_cast<std::underlying_type_t<decltype(T1)> >(T1) - static_cast<std::underlying_type_t<decltype(T1)> >(T2);
};

template <auto T1, auto T2>
struct diff_of<T1,
               T2,
               std::enable_if_t<std::is_same_v<decltype(T1), decltype(T2)> && std::is_integral_v<decltype(T1)>, void> >
{
	static constexpr auto value = T1 - T2;
};

template <auto T1, auto T2>
static inline constexpr auto diff_of_v = diff_of<T1, T2>::value;

template <auto T1, auto T2, typename T = void>
struct sum_of
{
};
template <auto T1, auto T2>
struct sum_of<T1, T2, std::enable_if_t<std::is_integral_v<decltype(T1)>, void> >
{
	static constexpr auto value = T1 + T2;
};
template <auto T1, auto T2>
struct sum_of<T1, T2, std::enable_if_t<std::is_enum_v<decltype(T1)>, void> >
{
	static constexpr auto value =
	    static_cast<decltype(T1)>(static_cast<std::underlying_type_t<decltype(T1)> >(T1) + T2);
};
template <auto T1, int T2>
static inline constexpr auto sum_of_v = sum_of<T1, T2>::value;

template <typename... Args>
struct to_value_sequence
{
};

template <typename T, T... values>
struct to_value_sequence<mp11::mp_list<std::integral_constant<T, values>...> >
{
	using type = value_sequence<T, values...>;
};

template <typename... Args>
struct union_of_helper
{
	using type = mp11::mp_list<>;
};

template <auto T1, auto T2, typename... Args>
struct union_of_helper<index_range<T1, T2>, Args...>
{
	template <class T>
	using add_value = std::integral_constant<std::decay_t<decltype(T1)>, sum_of_v<T1, static_cast<int>(T::value)> >;
	using type = mp11::mp_append<mp11::mp_transform<add_value, mp11::mp_iota_c<diff_of_v<T2, T1> + 1> >,
	                             typename union_of_helper<Args...>::type>;
};

template <auto T1, typename... Args>
struct union_of_helper<std::integral_constant<decltype(T1), T1>, Args...>
{
	using type = mp11::mp_append<mp11::mp_list<std::integral_constant<std::decay_t<decltype(T1)>, T1> >,
	                             typename union_of_helper<Args...>::type>;
};

template <typename... Args>
using union_of = typename to_value_sequence<typename union_of_helper<Args...>::type>::type;

template <auto T1>
using single_value = std::integral_constant<decltype(T1), T1>;

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_UNION_OF_H
