// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_INDEX_RANGE_H
#define JBC_INDEXED_ARRAY_DETAIL_INDEX_RANGE_H

#include "is_contiguous.hpp"

namespace jbc::indexed_array::detail
{

template <class T>
struct type_identity
{
	using type = T;
};

template <class T>
using type_identity_t = typename type_identity<T>::type;

template <auto minInclusive, type_identity_t<decltype(minInclusive)> maxInclusive>
struct index_range
{
	using type = decltype(minInclusive);
	static inline constexpr type const min = minInclusive;
	static inline constexpr type const max = maxInclusive;
	static_assert(integral_value_v<minInclusive> <= integral_value_v<maxInclusive>,
	              "Bounds must form a non-empty range, min <= max");
};

template <auto minInclusive, type_identity_t<decltype(minInclusive)> maxInclusive>
using interval = index_range<minInclusive, maxInclusive>;

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_INDEX_RANGE_H
