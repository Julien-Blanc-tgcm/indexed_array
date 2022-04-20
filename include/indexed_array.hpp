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
constexpr auto safe_arg(T&& val)
{
	return detail::checked_arg_h<typename detail::integral_args<C...>::type, T>(std::forward<T>(val));
}

} // namespace jbc::indexed_array

#endif // JBC_INDEXED_ARRAY_H
