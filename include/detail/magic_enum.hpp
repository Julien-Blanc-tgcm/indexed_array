// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_MAGIC_ENUM_H
#define JBC_INDEXED_ARRAY_MAGIC_ENUM_H

#include "default_indexer.hpp"

#include <magic_enum.hpp>

namespace jbc::indexed_array::detail
{

/*template <typename... Args>
struct magic_enum_to_integer_sequence
{
};

template <typename Enum, template <class...> typename L, typename... Args>
struct magic_enum_to_integer_sequence<Enum, L<Args...> >
{
	using type = std::integer_sequence<Enum, Args::value...>;
}; */

template <typename T>
using has_magic_enum = std::is_enum<T>;

template <typename Enum>
struct default_indexer<Enum, typename std::enable_if_t<has_magic_enum<Enum>::value, void> >
{
	using helper_list_type = magic_enum::values_sequence<Enum>;
	using index = Enum;
	static inline constexpr auto const size = default_indexer<helper_list_type>::size;
	template <bool throws_on_error = false>
	static constexpr auto at(Enum v) noexcept(!throws_on_error)
	{
		return default_indexer<helper_list_type>::template at<throws_on_error>(v);
	}
};

} // jbc::indexed_array::detail
#endif // JBC_INDEXED_ARRAY_MAGIC_ENUM_H

