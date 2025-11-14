// Copyright 2025 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_LIST_INDEXER_H
#define JBC_INDEXED_ARRAY_DETAIL_LIST_INDEXER_H

#include "default_indexer.hpp"

#include <stdexcept>
#include <type_traits>

#include <boost/config.hpp> // for BOOST_UNLIKELY
#include <boost/mp11.hpp>

namespace jbc::indexed_array::detail
{

template <typename Type, typename List>
struct type_in_list
{
	constexpr static bool in_list()
	{
		bool ret = false;
		boost::mp11::mp_for_each<List>([&ret](auto C) {
			if constexpr (std::is_same_v<std::decay_t<Type>, std::decay_t<decltype(C.value)>>)
				ret = true;
		});
		return ret;
	}
	constexpr static bool value = in_list();
};

template <typename L>
    requires(boost::mp11::mp_is_value_list<L>::value)
struct list_indexer
{
  public:
	static constexpr auto const size = boost::mp11::mp_size<L>::value;
	static constexpr bool is_o1 = false;

	template <bool b, typename T>
	static constexpr std::size_t at(T val) noexcept(!b)
	    requires(type_in_list<T, L>::value)
	{
		bool valueFound = false;
		std::size_t ret = 0;
		boost::mp11::mp_for_each<L>([&](auto C) {
			if (valueFound)
				return;
			if constexpr (std::is_same_v<std::decay_t<T>, std::decay_t<decltype(C.value)>>)
			{
				if (C.value == val)
					valueFound = true;
				else
					ret += 1;
			}
			else
				ret += 1;
		});
		if (valueFound)
			return ret;
		if constexpr (b)
		{
			if (b)
				throw std::out_of_range("invalid index");
		}
		return ret;
	}

	template <typename T>
	static constexpr bool in_range(T val)
	    requires(type_in_list<T, L>::value)
	{
		bool valueFound = false;
		std::size_t ret = 0;
		boost::mp11::mp_for_each<L>([&](auto C) {
			if (valueFound)
				return;
			if constexpr (std::is_same_v<std::decay_t<T>, std::decay_t<decltype(C.value)>>)
			{
				if (C.value == val)
					valueFound = true;
				else
					ret += 1;
			}
			else
				ret += 1;
		});
		if (valueFound)
			return ret;
		return false;
	}
};

template <auto... V>
struct value_list
{
};

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_LIST_INDEXER_H
