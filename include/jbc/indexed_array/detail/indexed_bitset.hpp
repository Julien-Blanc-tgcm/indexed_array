// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_INDEXED_BITSET_H
#define JBC_INDEXED_ARRAY_DETAIL_INDEXED_BITSET_H

#include "checked_arg.hpp"
#include "default_indexer.hpp"

#include <boost/mp11.hpp>

#include <bit>
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
template <std::size_t size>
struct underlying_type_small
{
};

template <std::size_t size>
    requires(size <= 8)
struct underlying_type_small<size>
{
	using storage = std::uint8_t;
	using type = std::uint8_t;
};
template <std::size_t size>
    requires(size > 8 && size <= 64)
struct underlying_type_small<size>
{
	using storage = std::uint64_t;
	using type = std::uint64_t;
};

template <std::size_t size>
    requires(size > 32 && size % 32 == 0 && size % 64 != 0)
struct underlying_type_small<size>
{
	static constexpr std::size_t needed_size_ = size / (sizeof(std::uint32_t) * 8);
	using storage = std::array<std::uint32_t, needed_size_>;
	using type = std::uint32_t;
};

template <std::size_t size>
    requires(size > 64 && size % 64 == 0)
struct underlying_type_small<size>
{
	static constexpr std::size_t needed_size_ = size / (sizeof(std::uint64_t) * 8);
	using storage = std::array<std::uint64_t, needed_size_>;
	using type = std::uint64_t;
};

template <std::size_t size>
    requires(size > 64 && size % 32 != 0)
struct underlying_type_small<size>
{
	static constexpr std::size_t unit_size_ = sizeof(std::uint8_t) * 8;
	static constexpr std::size_t needed_size_ = size / unit_size_ + ((size % unit_size_ == 0) ? 0 : 1);
	using storage = std::array<std::uint8_t, needed_size_>;
	using type = std::uint8_t;
};

template <jbc::indexed_array::concepts::indexer Indexer, typename underlying = underlying_type_small<Indexer::size>>
class indexed_bitset
{
	using item_type = typename underlying::type;
	using storage_type = typename underlying::storage;
	using underlying_type = typename underlying::type;
	storage_type d_{};

  public:
	struct reference
	{
		indexed_bitset& ref;
		std::size_t index;
		constexpr operator bool() const
		{
			return ref.test_(index);
		}
		constexpr reference& operator=(bool v)
		{
			if (v)
				ref.set_(index);
			else
				ref.reset_(index);
			return *this;
		}
	};
	// standard bitset members
	using value_type = bool;
	using size_type = std::size_t;
	using const_reference = bool;

	// specific
	using indexer = Indexer;

	constexpr indexed_bitset() = default;

	constexpr ~indexed_bitset() = default;

	constexpr indexed_bitset(indexed_bitset<Indexer> const& other) : d_{other.d_}
	{
	}

	constexpr indexed_bitset(indexed_bitset<Indexer>&& other) : d_{other.d_}
	{
	}

	template <typename T>
	    requires(std::is_unsigned_v<T>)
	constexpr indexed_bitset(T v) noexcept : d_{}
	{
		if constexpr (std::is_unsigned_v<storage_type>)
		{
			d_ = static_cast<storage_type>(v);
		}
		else
		{
			for (auto i = 0u; i < sizeof(T) && i < d_.size(); ++i)
			{
				d_[i] = static_cast<std::byte>((v >> (i * 8u)) & 0xFFu);
			}
		}
	}

	constexpr indexed_bitset& operator=(indexed_bitset<Indexer> const& other)
	{
		d_ = other.d_;
		return *this;
	}

	constexpr indexed_bitset& operator=(indexed_bitset<Indexer>&& other)
	{
		d_ = other.d_;
		return *this;
	}

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

	// [] operator and standard functions. Use concepts to disable overloads that won't work
	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr bool test(Args&&... args) const
	{
		return test_(Indexer::template at<true>(std::forward<Args>(args)...));
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr indexed_bitset& set(Args&&... args)
	{
		set_(Indexer::template at<true>(std::forward<Args>(args)...));
		return *this;
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr indexed_bitset& reset(Args&&... args)
	{
		reset_(Indexer::template at<true>(std::forward<Args>(args)...));
		return *this;
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr indexed_bitset& flip(Args&&... args)
	{
		flip_(Indexer::template at<true>(std::forward<Args>(args)...));
		return *this;
	}

#if defined(__cpp_multidimensional_subscript)
	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr reference operator[](Args&&... args)
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return reference{*this, i};
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr const_reference operator[](Args&&... args) const
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return test_(i);
	}
#endif

	template <typename Arg>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Arg>
	constexpr reference operator[](Arg&& arg)
	{
		auto i = Indexer::template at<false>(std::forward<Arg>(arg));
		return reference{*this, i};
	}

	template <typename Arg>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Arg>
	constexpr const_reference operator[](Arg&& arg) const
	{
		return test_(Indexer::template at<false>(std::forward<Arg>(arg)));
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr reference operator[](std::tuple<Args...> arg)
	{
		auto f = static_cast<std::size_t (*)(std::decay_t<Args>...)>(Indexer::template at<false>);
		auto i = std::apply(f, std::forward<std::tuple<Args...>>(arg));
		return reference{*this, i};
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr const_reference operator[](std::tuple<Args...> arg) const
	{
		auto f = static_cast<std::size_t (*)(std::decay_t<Args>...)>(Indexer::template at<false>);
		auto i = std::apply(f, std::forward<std::tuple<Args...>>(arg));
		return test_(i);
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr reference operator()(Args&&... args)
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return reference{*this, i};
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr const_reference operator()(Args&&... args) const
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return test_(i);
	}

	template <typename... Args>
	    requires(jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>)
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
		if constexpr (std::is_unsigned_v<storage_type>)
		{
			return std::popcount(d_);
		}
		else
		{
			std::size_t res = 0;
			constexpr auto size = std::tuple_size<decltype(d_)>::value;

			constexpr auto tcount = size;
			constexpr auto s = size % sizeof(underlying_type);
			constexpr auto r = size / sizeof(underlying_type);
			for (auto count = 0; count < tcount; ++count)
			{
				res += std::popcount(d_[count]);
			}
			return res;
		}
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
		if constexpr (std::is_unsigned_v<storage_type>)
		{
			return d_ == 0;
		}
		else
		{
			for (auto c : d_)
			{
				if (static_cast<underlying_type>(c) != 0)
					return false;
			}
			return true;
		}
	}

	template <typename T>
	    requires(std::is_unsigned_v<T> && sizeof(T) * 8 >= Indexer::size)
	constexpr T to() const
	{
		if constexpr (std::is_unsigned_v<storage_type>)
		{
			return static_cast<T>(d_);
		}
		else
		{
			T t{0u};
			for (std::size_t i = 0; i < d_.size(); ++i)
				t |= static_cast<T>(d_[i]) << (8u * i * sizeof(underlying_type));
			return t;
		}
	}

  private:
	constexpr bool test_(std::size_t i) const
	{
		if constexpr (std::is_unsigned_v<storage_type>)
		{
			return static_cast<bool>((d_ >> i) & 0x1u);
		}
		else
		{
			auto s = static_cast<underlying_type>(i / (8u * sizeof(underlying_type)));
			auto r = static_cast<underlying_type>(i % (8u * sizeof(underlying_type)));
			return (static_cast<underlying_type>(d_[s]) >> r) & 0x1u;
		}
	}

	constexpr void set_(std::size_t i)
	{
		if constexpr (std::is_unsigned_v<storage_type>)
		{
			d_ |= (0x1u << i);
		}
		else
		{
			auto s = static_cast<underlying_type>(i / (8u * sizeof(underlying_type)));
			auto r = static_cast<underlying_type>(i % (8u * sizeof(underlying_type)));
			auto v = static_cast<underlying_type>(d_[s]);
			v |= (underlying_type(1u) << r);
			d_[s] = static_cast<item_type>(v);
		}
	}

	constexpr void reset_(std::size_t i)
	{
		if constexpr (std::is_unsigned_v<storage_type>)
		{
			d_ &= ~(static_cast<storage_type>(storage_type(0x1u) << i));
		}
		else
		{
			auto s = static_cast<underlying_type>(i / (8u * sizeof(underlying_type)));
			auto r = static_cast<underlying_type>(i % (8u * sizeof(underlying_type)));
			auto v = static_cast<underlying_type>(d_[s]);
			v &= ~(static_cast<underlying_type>(1u) << r);
			d_[s] = static_cast<item_type>(v);
		}
	}

	constexpr void flip_(std::size_t i)
	{
		if constexpr (std::is_unsigned_v<storage_type>)
		{
			d_ ^= (storage_type(0x1u) << i);
		}
		else
		{
			auto s = static_cast<underlying_type>(i / (8u * sizeof(underlying_type)));
			auto r = static_cast<underlying_type>(i % (8u * sizeof(underlying_type)));
			auto v = static_cast<underlying_type>(d_[s]);
			v ^= (underlying_type(1u) << r);
			d_[s] = static_cast<item_type>(v);
		}
	}
	constexpr friend bool operator==(indexed_bitset const& f, indexed_bitset const& s)
	{
		return f.d_ == s.d_;
	}
};


} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_INDEXED_ARRAY_H
