//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef IS_CONTIGUOUS_HPP
#define IS_CONTIGUOUS_HPP

#include <boost/mp11.hpp>

#include <cmath>
#include <type_traits>
#include <utility>

namespace jbc::indexed_array::detail
{

template <typename T, T value, typename U = void>
struct integral_value
{
};

template <typename T, T v>
struct integral_value<T, v, std::enable_if_t<std::is_integral<T>::value, void> >
{
	static inline constexpr T value = v;
};

template <typename T, T v>
struct integral_value<T, v, std::enable_if_t<std::is_enum<T>::value, void> >
{
	static inline constexpr typename std::underlying_type<T>::type value =
	    static_cast<typename std::underlying_type<T>::type>(v);
};

template <auto C>
static inline constexpr auto const integral_value_v = integral_value<decltype(C), C>::value;

template <typename... Args>
struct is_contiguous_sequence : public std::false_type
{
};

template <typename Arg1, typename Arg2, typename... Args>
struct is_contiguous_sequence<Arg1, Arg2, Args...> :
    public std::conditional<
        std::is_same<std::decay_t<decltype(Arg1::value)>, std::decay_t<decltype(Arg2::value)> >::value &&
            (integral_value_v<Arg2::value> - integral_value_v<Arg1::value>) <= 1 && Arg1::value <= Arg2::value,
        is_contiguous_sequence<Arg2, Args...>,
        std::false_type>::type
{
};

template <typename Last>
struct is_contiguous_sequence<Last> : public std::true_type
{
};

template <template <typename...> class List, typename... Args>
struct is_contiguous_sequence<List<Args...> > : public is_contiguous_sequence<Args...>
{
};

} // namespace jbc::indexed_array::detail

#endif // IS_CONTIGUOUS_HPP
