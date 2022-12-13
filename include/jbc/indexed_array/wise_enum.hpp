// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_WISE_ENUM_H
#define JBC_INDEXED_ARRAY_WISE_ENUM_H

#include "detail/default_indexer.hpp"

#include <wise_enum/wise_enum.h>

namespace jbc::indexed_array::detail
{

template <typename E, typename I>
struct wise_enum_values_sequence_helper
{
};

template <typename E, std::size_t... I>
struct wise_enum_values_sequence_helper<E, std::integer_sequence<std::size_t, I...> >
{
	using type = value_sequence<E, wise_enum::enumerators<E>::range[I].value...>;
};

template <typename E>
using wise_enum_values_sequence =
    typename wise_enum_values_sequence_helper<E, std::make_index_sequence<wise_enum::enumerators<E>::size> >::type;

template <typename Enum>
struct default_indexer<Enum, typename std::enable_if_t<wise_enum::is_wise_enum<Enum>::value, void> >
{
	using helper_list_type = wise_enum_values_sequence<Enum>;
	using index = Enum;
	static inline constexpr auto const size = default_indexer<helper_list_type>::size;
	template <bool throws_on_error = false>
	static constexpr auto at(Enum v) noexcept(!throws_on_error)
	{
		return default_indexer<helper_list_type>::template at<throws_on_error>(v);
	}
	static constexpr auto in_range(Enum v) noexcept
	{
		return default_indexer<helper_list_type>::in_range(v);
	}
	static constexpr bool is_o1 = default_indexer<helper_list_type>::is_o1;
};

template <typename Enum>
struct create_list_helper<
    default_indexer<Enum, typename std::enable_if_t<wise_enum::is_wise_enum<Enum>::value, void> > >
{
	using type = typename create_list_helper<default_indexer<typename default_indexer<Enum>::helper_list_type> >::type;
};

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_WISE_ENUM_H
