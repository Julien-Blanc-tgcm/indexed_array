// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_DESCRIBE_H
#define JBC_INDEXED_ARRAY_DETAIL_DESCRIBE_H

#include "default_indexer.hpp"

#include <boost/describe.hpp>

namespace jbc::indexed_array::detail
{

template <typename... Args>
struct describe_to_value_sequence
{
};

template <typename Enum, template <class...> typename L, typename... Args>
struct describe_to_value_sequence<Enum, L<Args...> >
{
	using type = value_sequence<Enum, Args::value...>;
};

template <typename Enum>
struct default_indexer<Enum, typename std::enable_if_t<boost::describe::has_describe_enumerators<Enum>::value, void> >
{
	using helper_list_type =
	    typename describe_to_value_sequence<Enum, boost::describe::describe_enumerators<Enum> >::type;
	using index = Enum;
	static inline constexpr auto const size = default_indexer<helper_list_type>::size;
	template <bool throws_on_error = false>
	static constexpr auto at(Enum v) noexcept(!throws_on_error)
	{
		return default_indexer<helper_list_type>::template at<throws_on_error>(v);
	}
};

template <typename Enum>
struct create_list_helper<
    default_indexer<Enum, typename std::enable_if_t<boost::describe::has_describe_enumerators<Enum>::value, void> > >
{
	using type = typename create_list_helper<default_indexer<typename default_indexer<Enum>::helper_list_type> >::type;
};

} // namespace jbc::indexed_array::detail
#endif // JBC_INDEXED_ARRAY_DETAIL_DESCRIBE_H
