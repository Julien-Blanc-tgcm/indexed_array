// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_FOREACH_H
#define JBC_INDEXED_ARRAY_DETAIL_FOREACH_H

#include "default_indexer.hpp"
#include "indexed_array.hpp"
#include "union_of.hpp"

#include <boost/mp11.hpp>
#if defined(INDEXED_ARRAY_HAS_REFLECTION)
#include <meta>
#endif

namespace jbc::indexed_array::detail
{

template <typename T>
struct create_list_helper
{
};

template <auto first, type_identity_t<decltype(first)>... idx>
struct create_list_helper<default_indexer<value_sequence<decltype(first), first, idx...> > >
{
	using type = boost::mp11::mp_list<std::integral_constant<decltype(first), first>,
	                                  std::integral_constant<decltype(first), idx>...>;
};

template <auto v1, type_identity_t<decltype(v1)> v2>
struct create_list_helper<default_indexer<index_range<v1, v2> > >
{
	using type = typename union_of_helper<index_range<v1, v2> >::type;
};

template <typename T, typename Func>
struct for_each_ // default implementation, does nothing
{
};


template <typename Content, typename Indexer, typename Func>
struct for_each_<indexed_array<Content, Indexer>, Func>
{
	static void apply(Func&& f, indexed_array<Content, Indexer>& data)
	{
		using list_type = typename create_list_helper<Indexer>::type;
		auto current = data.begin();
		boost::mp11::mp_for_each<list_type>([&current, f](auto I) {
			f(I, *current);
			++current;
		});
	}
	static void apply(Func&& f, indexed_array<Content, Indexer> const& data)
	{
		using list_type = typename create_list_helper<Indexer>::type;
		auto current = data.begin();
		boost::mp11::mp_for_each<list_type>([&current, f](auto I) {
			f(I, *current);
			++current;
		});
	}
};
#if defined(INDEXED_ARRAY_HAS_REFLECTION)
template <typename Content, typename Enum, typename Func>
requires (std::is_enum_v<Enum>
#if defined(INDEXED_ARRAY_HAS_DESCRIBE)
		&& !boost::describe::has_describe_enumerators<Enum>::value
#endif
	 )
struct for_each_<indexed_array<Content, detail::default_indexer<Enum>>, Func>
{
	static void apply(Func&& f, indexed_array<Content, detail::default_indexer<Enum>>& data)
	{
		auto current = data.begin();
		template for(constexpr auto m : std::define_static_array(std::meta::enumerators_of(^^Enum)))
		{
			f([:m:], *current);
			++current;
		}
	}
	static void apply(Func&& f, indexed_array<Content, detail::default_indexer<Enum>> const& data)
	{
		auto current = data.begin();
		template for(constexpr auto m : std::define_static_array(std::meta::enumerators_of(^^Enum)))
		{
			f([:m:], *current);
			++current;
		}
	}
};
#endif

template <typename Content, typename Indexer, typename Func>
struct for_each_<indexed_span<Content, Indexer>, Func>
{
	static void apply(Func&& f, indexed_span<Content, Indexer>& data)
	{
		using list_type = typename create_list_helper<Indexer>::type;
		auto current = data.begin();
		boost::mp11::mp_for_each<list_type>([&current, f](auto I) {
			f(I, *current);
			++current;
		});
	}
	static void apply(Func&& f, indexed_span<Content, Indexer> const& data)
	{
		using list_type = typename create_list_helper<Indexer>::type;
		auto current = data.begin();
		boost::mp11::mp_for_each<list_type>([&current, f](auto I) {
			f(I, *current);
			++current;
		});
	}
};

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_FOREACH_H
