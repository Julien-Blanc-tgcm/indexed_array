//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_H
#define JBC_INDEXED_ARRAY_H

#include "detail/checked_arg.hpp"
#include "detail/default_indexer.hpp"
#include "detail/indexed_array.hpp"
#include "detail/is_contiguous.hpp"
#include "detail/union_of.hpp"

namespace jbc::indexed_array
{

namespace detail
{
template <typename T, typename = void>
struct add_default_handler_if_needed
{
	using type = default_indexer<T>;
};
template <typename T>
struct add_default_handler_if_needed<T, std::enable_if_t<has_member_size<T>::value, void> >
{
	using type = T;
};

template <typename T>
using add_default_handler_if_needed_t = typename add_default_handler_if_needed<T>::type;

template <typename... Args>
struct to_single_indexer
{
	using type = default_indexer<boost::mp11::mp_list<add_default_handler_if_needed_t<Args>...> >;
};

template <typename Arg>
struct to_single_indexer<Arg>
{
	using type = add_default_handler_if_needed_t<Arg>;
};

template <typename... Args>
using to_single_indexer_t = typename to_single_indexer<Args...>::type;

}

template <typename Value, typename... Index>
using indexed_array = detail::indexed_array<Value, detail::to_single_indexer_t<Index...> >;

using detail::interval;
using detail::single_value;
using detail::union_of;

namespace detail
{
template <auto... C>
struct integral_args
{
	using type = boost::mp11::mp_list<std::integral_constant<decltype(C), C>...>;
};
template <auto C>
struct integral_args<C>
{
	using type = std::integral_constant<decltype(C), C>;
};
} // namespace detail

template <auto... C, typename T>
auto safe_arg(T&& val)
{
	return detail::checked_arg_h<typename detail::integral_args<C...>::type, T>(std::forward<T>(val));
}

} // namespace jbc::indexed_array

#endif // JBC_INDEXED_ARRAY_H
