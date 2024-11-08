// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_INDEXED_BITSET_H
#define JBC_INDEXED_ARRAY_DETAIL_INDEXED_BITSET_H

#include "checked_arg.hpp"
#include "default_indexer.hpp"

#include <boost/mp11.hpp>

#include <bitset>
#include <type_traits>

namespace jbc::indexed_array::detail
{

template <typename Arg, typename... Args>
unsigned long long bitset_safe_arg_sum(unsigned long long offset, Arg&& arg, Args&&... args)
{
	if constexpr (sizeof...(Args) == 0)
		return static_cast<unsigned long long>(static_cast<bool>(arg)) << offset;
	else
		return (static_cast<unsigned long long>(static_cast<bool>(arg)) << offset) +
		       bitset_safe_arg_sum<Args...>(offset + 1, std::forward<Args>(args)...);
}

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
template <jbc::indexed_array::concepts::indexer Indexer>
#else
template <typename Indexer>
#endif
class indexed_bitset
{
	std::bitset<Indexer::size> data_;

  public:
	// standard bitset members
	using value_type = bool;
	using size_type = std::size_t;
	using reference = typename decltype(data_)::reference;
	using const_reference = bool;

	// specific
	using indexer = Indexer;

	constexpr indexed_bitset() = default;

	~indexed_bitset() = default;

	constexpr indexed_bitset(indexed_bitset<Indexer> const& other) = default;

	constexpr indexed_bitset(indexed_bitset<Indexer>&& other) = default;

	constexpr indexed_bitset(unsigned long long v) noexcept : data_{v}
	{
	}

	constexpr indexed_bitset& operator=(indexed_bitset<Indexer> const& other) = default;

	constexpr indexed_bitset& operator=(indexed_bitset<Indexer>&& other) = default;

	constexpr explicit operator std::bitset<Indexer::size> const&() const
	{
		return data_;
	}

	constexpr explicit operator std::bitset<Indexer::size>&()
	{
		return data_;
	}

	// safe_arg constructor
	template <typename Arg,
	          typename... Args,
	          std::enable_if_t<is_checked_arg<Arg>::value && sizeof...(Args) < sizeof(unsigned long long), bool> = true>
	constexpr explicit indexed_bitset(Arg&& head, Args&&... args) noexcept :
	    data_(bitset_safe_arg_sum<Arg, Args...>(0, std::forward<Arg>(head), std::forward<Args>(args)...))
	{
		static_assert(
		    detail::correct_index<Indexer, typename Arg::checked_arg_index, typename Args::checked_arg_index...>(),
		    "Argument mismatch");
	}

	// [] operator and standard functions. Use enable_if to disable overloads that won't work
#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename... Args>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
#else
	template <typename... Args, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Args...> > >
#endif
	    constexpr bool test(Args&&... args) const
	{
		return data_.test(Indexer::template at<true>(std::forward<Args>(args)...));
	}

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename... Args>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
#else
	template <typename... Args, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Args...> > >
#endif
	    constexpr indexed_bitset& set(Args&&... args)
	{
		data_.set(Indexer::template at<true>(std::forward<Args>(args)...));
		return *this;
	}

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename... Args>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
#else
	template <typename... Args, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Args...> > >
#endif
	    constexpr indexed_bitset& reset(Args&&... args)
	{
		data_.reset(Indexer::template at<true>(std::forward<Args>(args)...));
		return *this;
	}

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename... Args>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
#else
	template <typename... Args, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Args...> > >
#endif
	    constexpr indexed_bitset& flip(Args&&... args)
	{
		data_.flip(Indexer::template at<true>(std::forward<Args>(args)...));
		return *this;
	}

#if defined(__cpp_multidimensional_subscript)
#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename... Args>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
#else
	template <typename... Args, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Args...> > >
#endif
	    constexpr reference operator[](Args&&... args)
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return data_[i];
	}

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename... Args>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
#else
	template <typename... Args, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Args...> > >
#endif
	    constexpr const_reference operator[](Args&&... args) const
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return data_[i];
	}
#else
#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename Arg>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Arg>
#else
	template <typename Arg, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Arg> > >
#endif
	    constexpr reference operator[](Arg&& arg)
	{
		return data_[Indexer::template at<false>(std::forward<Arg>(arg))];
	}

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename Arg>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Arg>
#else
	template <typename Arg, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Arg> > >
#endif
	    constexpr const_reference operator[](Arg&& arg) const
	{
		return data_[Indexer::template at<false>(std::forward<Arg>(arg))];
	}
#endif

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename... Args>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
#else
	template <typename... Args, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Args...> > >
#endif
	    constexpr reference operator[](std::tuple<Args...> arg)
	{
		auto f = static_cast<std::size_t (*)(std::decay_t<Args>...)>(Indexer::template at<false>);
		auto i = std::apply(f, std::forward<std::tuple<Args...> >(arg));
		return data_[i];
	}

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename... Args>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
#else
	template <typename... Args, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Args...> > >
#endif
	    constexpr const_reference operator[](std::tuple<Args...> arg) const
	{
		auto f = static_cast<std::size_t (*)(std::decay_t<Args>...)>(Indexer::template at<false>);
		auto i = std::apply(f, std::forward<std::tuple<Args...> >(arg));
		return data_[i];
	}

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename... Args>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
#else
	template <typename... Args, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Args...> > >
#endif
	    constexpr reference operator()(Args&&... args)
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return data_[i];
	}

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename... Args>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
#else
	template <typename... Args, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Args...> > >
#endif
	    constexpr const_reference operator()(Args&&... args) const
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return data_[i];
	}


#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename... Args>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
#else
	template <typename... Args, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Args...> > >
#endif
	    constexpr bool in_range(Args&&... args)
	{
		return Indexer::in_range(std::forward<Args>(args)...);
	}

	static constexpr bool is_o1 = Indexer::is_o1;

	constexpr size_type size() const
	{
		return Indexer::size;
	}

	constexpr std::size_t count() const
	{
		return data_.count();
	}

	constexpr bool all() const
	{
		return data_.all();
	}

	constexpr bool any() const
	{
		return data_.any();
	}

	constexpr bool none() const
	{
		return data_.none();
	}

	unsigned long to_ulong() const
	{
		return data_.to_ulong();
	}

	unsigned long long to_ullong() const
	{
		return data_.to_ullong();
	}
};

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_INDEXED_ARRAY_H
