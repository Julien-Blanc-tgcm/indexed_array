// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_SAFE_NUMERICS_H
#define JBC_INDEXED_ARRAY_DETAIL_SAFE_NUMERICS_H

#include "detail/default_indexer.hpp"

#include <boost/safe_numerics/safe_integer_range.hpp>

namespace jbc::indexed_array
{

template<int min, int max>
struct safe_signed_range_indexer
{
	static constexpr auto size = max - min + 1;

	template <bool>
	static constexpr std::size_t at(boost::safe_numerics::safe_signed_range<min, max> v) noexcept
	{
		// ignore b: we will always be noexcept, as safe_signed_range guarantees it
		return static_cast<std::size_t>(v - min);
	}

	static constexpr bool is_o1 = true;
};

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_SAFE_NUMERICS_H
