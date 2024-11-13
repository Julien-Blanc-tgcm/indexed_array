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
//	std::bitset<Indexer::size> data_;
    std::array<std::byte, Indexer::size / 8 + ((Indexer::size % 8 == 0)? 0 : 1)> d_;

  public:
	struct reference {
		indexed_bitset& ref;
		std::size_t index;
		constexpr operator bool() const {
			return ref.test_(index);
		}
		constexpr reference& operator=(bool v) {
			if (v)
				ref.set_(index);
			else
				ref.reset_(index);
			return *this;
		}
/*		constexpr bool operator!() const {
			return !ref.test_(index);
		} */
	};
	// standard bitset members
	using value_type = bool;
	using size_type = std::size_t;
//	using reference = typename decltype(data_)::reference;
	using const_reference = bool;

	// specific
	using indexer = Indexer;

	constexpr indexed_bitset() = default;

	~indexed_bitset() = default;

	constexpr indexed_bitset(indexed_bitset<Indexer> const& other) = default;

	constexpr indexed_bitset(indexed_bitset<Indexer>&& other) = default;

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename T>
        requires (std::is_unsigned_v<T>)
#else
	template <typename T, typename E = std::enable_if_t<std::is_unsigned_v<T>>>
#endif
	constexpr indexed_bitset(T v) noexcept : d_{}
	{
		for (auto i = 0u; i < sizeof(T) && i < d_.size(); ++i)
		{
			d_[i] = static_cast<std::byte>((v >> (i * 8u)) & 0xFFu);
		}
	}

	constexpr indexed_bitset& operator=(indexed_bitset<Indexer> const& other) = default;

	constexpr indexed_bitset& operator=(indexed_bitset<Indexer>&& other) = default;

	// safe_arg constructor
	template <typename Arg,
	          typename... Args,
	          std::enable_if_t<is_checked_arg<Arg>::value && sizeof...(Args) < sizeof(unsigned long long), bool> = true>
	constexpr explicit indexed_bitset(Arg&& head, Args&&... args) noexcept :
	    indexed_bitset(bitset_safe_arg_sum<Arg, Args...>(0, std::forward<Arg>(head), std::forward<Args>(args)...))
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
		return test_(Indexer::template at<true>(std::forward<Args>(args)...));
	}

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename... Args>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
#else
	template <typename... Args, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Args...> > >
#endif
	    constexpr indexed_bitset& set(Args&&... args)
	{
		set_(Indexer::template at<true>(std::forward<Args>(args)...));
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
		reset_(Indexer::template at<true>(std::forward<Args>(args)...));
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
		flip_(Indexer::template at<true>(std::forward<Args>(args)...));
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
		return reference{*this, i};
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
		return test_(i);
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
		auto i = Indexer::template at<false>(std::forward<Arg>(arg));
		return reference{*this, i};
	}

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename Arg>
	requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Arg>
#else
	template <typename Arg, typename T = std::enable_if_t<is_indexer_invocable_with_v<indexer, Arg> > >
#endif
	    constexpr const_reference operator[](Arg&& arg) const
	{
		return test_(Indexer::template at<false>(std::forward<Arg>(arg)));
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
		return reference{*this, i};
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
		return test_(i);
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
		return reference{*this, i};
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
		return test_(i);
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
		std::size_t res = 0;
		for (auto i = 0u; i < size(); ++i)
		{
			if (test_(i))
				res += 1;
		}
		return res;
	}

	constexpr bool all() const
	{
		return count() == size();
	}

	constexpr bool any() const
	{
		return !none();
	}

	constexpr bool none() const
	{
		for (auto c : d_)
		{
			if (static_cast<std::uint_fast8_t>(c) != 0)
				return false;
		}
		return true;
	}

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
	template <typename T>
        requires (std::is_unsigned_v<T> && sizeof(T) * 8 >= Indexer::size)
#else
	template <typename T, typename E = std::enable_if_t<std::is_unsigned_v<T> && sizeof(T) * 8 >= Indexer::size>>
#endif
	constexpr T to() const
	{
		T t{0u};
		for (std::size_t i = 0; i < d_.size(); ++i)
			t |= static_cast<T>(d_[i]) << (8u * i);
		return t;
	}

  private:
	constexpr bool test_(std::size_t i) const
	{
		auto s = i / 8u;
		auto r = i % 8u;
		return (static_cast<std::uint_fast8_t>(d_[s]) >> r) & 0x1u;
	}

	constexpr void set_(std::size_t i)
	{
		auto s = i / 8u;
		auto r = i % 8u;
		auto v = static_cast<std::uint_fast8_t>(d_[s]);
		v |= 1u << r;
		d_[s] = static_cast<std::byte>(v);
	}

	constexpr void reset_(std::size_t i)
	{
		auto s = i / 8u;
		auto r = i % 8u;
		auto v = static_cast<std::uint_fast8_t>(d_[s]);
		v &= ~(1u << r);
		d_[s] = static_cast<std::byte>(v);
	}

	constexpr void flip_(std::size_t i) 
	{
		auto s = i >> 3u; /// 8u;
		auto r = i & 0x7u; // % 8u;
		auto v = static_cast<std::uint_fast8_t>(d_[s]);
		v ^= (1u << r);
		d_[s] = static_cast<std::byte>(v);
	}
};

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_INDEXED_ARRAY_H
