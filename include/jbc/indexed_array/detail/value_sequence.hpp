// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_VALUE_SEQUENCE_H
#define JBC_INDEXED_ARRAY_DETAIL_VALUE_SEQUENCE_H

#include <type_traits>

namespace jbc::indexed_array::detail
{

template <typename T, T...>
struct value_sequence
{
	static_assert(std::is_enum<T>::value || std::is_integral<T>::value);
};

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_VALUE_SEQUENCE_H
