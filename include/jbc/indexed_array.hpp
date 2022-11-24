//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_H
#define JBC_INDEXED_ARRAY_H

#include "indexed_array/detail/checked_arg.hpp"
#include "indexed_array/detail/default_indexer.hpp"
#include "indexed_array/detail/foreach.hpp"
#include "indexed_array/detail/indexed_array.hpp"
#include "indexed_array/detail/is_contiguous.hpp"
#include "indexed_array/detail/union_of.hpp"

#if defined(INDEXED_ARRAY_HAS_MAGIC_ENUM) && defined(INDEXED_ARRAY_HAS_DESCRIBE)
#error "Compiling with both describe and magic enum support is not supported"
#endif
#if defined(INDEXED_ARRAY_HAS_MAGIC_ENUM) && defined(INDEXED_ARRAY_HAS_WISE_ENUM)
#error "Compiling with both wise enum and magic enum support is not supported"
#endif

#ifdef INDEXED_ARRAY_HAS_DESCRIBE
#include "indexed_array/describe.hpp"
#endif
#ifdef INDEXED_ARRAY_HAS_MAGIC_ENUM
#include "indexed_array/magic_enum.hpp"
#endif
#ifdef INDEXED_ARRAY_HAS_WISE_ENUM
#include "indexed_array/wise_enum.hpp"
#endif

namespace jbc::indexed_array
{

template <typename Value, typename Index1, typename... Indexes>
using indexed_array = detail::indexed_array<Value, detail::to_single_indexer_t<Index1, Indexes...> >;

using detail::interval;
using detail::single_value;
using detail::union_of;
template <typename Value, typename... Index>
using indexed_span = detail::indexed_span<Value, detail::to_single_indexer_t<Index...> >;

template <typename... Args>
using make_default_indexer = detail::to_single_indexer_t<Args...>;

template <typename T, typename Func>
void for_each(T&& container, Func&& func)
{
	return detail::for_each_<std::decay_t<T>, Func>::apply(std::forward<Func>(func), std::forward<T>(container));
}

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
