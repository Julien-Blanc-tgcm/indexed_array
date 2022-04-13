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

template <typename Value, typename Index>
using indexed_array = std::conditional_t<detail::has_member_size<detail::default_indexer<Index> >::value,
                                         detail::indexed_array<Value, detail::default_indexer<Index> >,
                                         detail::indexed_array<Value, Index> >;

using detail::interval;
using detail::single_value;
using detail::union_of;

template <auto C, typename T>
auto safe_arg(T&& val)
{
	return detail::checked_arg_h<C, T>(std::forward<T>(val));
}

} // namespace jbc::indexed_array

#endif // JBC_INDEXED_ARRAY_H
