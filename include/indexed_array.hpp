#ifndef JBC_INDEXED_ARRAY_H
#define JBC_INDEXED_ARRAY_H

#include "detail/checked_arg.hpp"
#include "detail/default_indexer.hpp"
#include "detail/indexed_array.hpp"
#include "detail/is_contiguous.hpp"

namespace jbc::indexed_array
{

template <typename Value, typename Index>
using indexed_array = std::conditional_t<detail::has_member_size<detail::default_indexer<Index> >::value,
                                         detail::indexed_array<Value, detail::default_indexer<Index> >,
                                         detail::indexed_array<Value, Index> >;

using detail::interval;

template <auto C, typename T>
auto safe_arg(T&& val)
{
	return detail::checked_arg_h<C, T>(std::forward<T>(val));
}

} // namespace jbc::indexed_array

#endif // JBC_INDEXED_ARRAY_H
