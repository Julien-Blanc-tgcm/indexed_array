// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_MAGIC_ENUM_H
#define JBC_INDEXED_ARRAY_MAGIC_ENUM_H

#include "default_indexer.hpp"

#include <magic_enum.hpp>

namespace jbc::indexed_array::detail
{

template <typename E, typename I>
struct values_sequence_helper
{
};

template <typename E, std::size_t... I>
struct values_sequence_helper<E, std::integer_sequence<std::size_t, I...> >
{
	using type = value_sequence<E, magic_enum::enum_value<E>(I)...>;
};

template <typename E>
using values_sequence =
    typename values_sequence_helper<E, std::make_index_sequence<magic_enum::enum_count<E>()> >::type;

template <typename T>
using has_magic_enum = std::is_enum<T>;

template <typename Enum>
struct default_indexer<Enum, typename std::enable_if_t<has_magic_enum<Enum>::value, void> >
{
	using helper_list_type = values_sequence<Enum>;
	using index = Enum;
	static inline constexpr auto const size = default_indexer<helper_list_type>::size;
	template <bool throws_on_error = false>
	static constexpr auto at(Enum v) noexcept(!throws_on_error)
	{
		return default_indexer<helper_list_type>::template at<throws_on_error>(v);
	}
};

template <typename Enum>
struct create_list_helper<default_indexer<Enum, typename std::enable_if_t<has_magic_enum<Enum>::value, void> > >
{
	using type = typename create_list_helper<default_indexer<typename default_indexer<Enum>::helper_list_type> >::type;
};

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_MAGIC_ENUM_H
