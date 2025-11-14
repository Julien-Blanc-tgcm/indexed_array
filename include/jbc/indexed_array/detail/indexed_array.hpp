// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_INDEXED_ARRAY_H
#define JBC_INDEXED_ARRAY_DETAIL_INDEXED_ARRAY_H

#include "checked_arg.hpp"
#include "default_indexer.hpp"
#include "indexed_span.hpp"

#include <boost/mp11.hpp>

#include <array>
#include <functional>
#include <type_traits>

namespace jbc::indexed_array::detail
{
template <typename Value, jbc::indexed_array::concepts::indexer Indexer>
class indexed_array
{
	std::array<Value, Indexer::size> data_;

  public:
	// standard array members
	using value_type = Value;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = value_type const&;
	using pointer = value_type*;
	using const_pointer = value_type const*;
	using iterator = typename std::array<Value, Indexer::size>::iterator;
	using const_iterator = typename std::array<Value, Indexer::size>::const_iterator;
	using reverse_iterator = typename std::array<Value, Indexer::size>::reverse_iterator;
	using const_reverse_iterator = typename std::array<Value, Indexer::size>::const_reverse_iterator;

	// specific
	using indexer = Indexer;

	constexpr indexed_array() = default;

	~indexed_array() = default;

	constexpr indexed_array(indexed_array<Value, Indexer> const& other) = default;

	constexpr indexed_array(indexed_array<Value, Indexer>&& other) = default;

	constexpr indexed_array& operator=(indexed_array<Value, Indexer> const& other) = default;

	constexpr indexed_array& operator=(indexed_array<Value, Indexer>&& other) = default;

	/**
	 * Constructor via an array of the same size and the same value content. Copy the array
	 * into the indexed_array content.
	 */
	constexpr indexed_array(std::array<Value, Indexer::size> const& other) noexcept(
	    std::is_nothrow_copy_constructible_v<Value>) : data_{other}
	{
	}

	// standard constructor
	/**
	 * Standard construction via a list of values. Behavior is the same as aggregate initialization
	 * of an std::array. No check is done on the number of values.
	 * @param list the list of values with which the array content will be initialized. Not that
	 * some values may be left uninitialized
	 */
	template <std::convertible_to<Value> Arg, std::convertible_to<Value>... Args>
	    requires(!(::jbc::indexed_array::concepts::checked_arg<Arg, Value> ||
	               (::jbc::indexed_array::concepts::checked_arg<Args, Value> || ...)))
	constexpr explicit indexed_array(Arg&& arg, Args&&... args) noexcept(std::is_nothrow_copy_constructible_v<Value>) :
	    data_{std::forward<Arg>(arg), std::forward<Args>(args)...}
	{
	}

	// safe_arg constructor
	/**
	 * @brief Specialization of the constructor that takes a `safe_arg` list of
	 * values. This is equivalent to aggregate initialization, but
	 * additional checks are done at compile time:
	 * @li the number of arguments must match with the array size
	 * @li the explicit index of each argument must match the order of the indexer
	 */
	template <::jbc::indexed_array::concepts::checked_arg<Value> Arg,
	          ::jbc::indexed_array::concepts::checked_arg<Value>... Args>
	constexpr explicit indexed_array(Arg&& head, Args&&... args) noexcept(std::is_nothrow_copy_constructible_v<Value>) :
	    data_{static_cast<Value>(head), static_cast<Value>(args)...}
	{
		static_assert(
		    detail::correct_index<Indexer, typename Arg::checked_arg_index, typename Args::checked_arg_index...>(),
		    "Argument mismatch");
	}

	// at and [] operators. Use enable_if to disable overloads that won't work
	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr reference at(Args&&... args)
	{
		return data_[Indexer::template at<true>(std::forward<Args>(args)...)];
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr const_reference at(Args&&... args) const
	{
		return data_[Indexer::template at<true>(std::forward<Args>(args)...)];
	}

#if defined(__cpp_multidimensional_subscript)
	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr reference operator[](Args&&... args)
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return data_[i];
	}
	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr const_reference operator[](Args&&... args) const
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return data_[i];
	}
#else
	// multiple arguments subscript not supported, restrict to single argument
	template <typename Arg>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Arg>
	constexpr reference operator[](Arg&& arg)
	{
		return data_[Indexer::template at<false>(std::forward<Arg>(arg))];
	}

	template <typename Arg>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Arg>
	constexpr const_reference operator[](Arg&& arg) const
	{
		return data_[Indexer::template at<false>(std::forward<Arg>(arg))];
	}
#endif

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr reference operator[](std::tuple<Args...> arg)
	{
		auto f = static_cast<std::size_t (*)(std::decay_t<Args>...)>(Indexer::template at<false>);
		auto i = std::apply(f, std::forward<std::tuple<Args...>>(arg));
		return data_[i];
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr const_reference operator[](std::tuple<Args...> arg) const
	{
		auto f = static_cast<std::size_t (*)(std::decay_t<Args>...)>(Indexer::template at<false>);
		auto i = std::apply(f, arg);
		return data_[i];
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr reference operator()(Args&&... args)
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return data_[i];
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr const_reference operator()(Args&&... args) const
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return data_[i];
	}

	template <typename Arg, std::enable_if_t<detail::has_root_indexer<Indexer, Arg>::value, int> = 0>
	constexpr auto slice(Arg&& idx)
	{
		return indexed_span<Value, typename Indexer::slice_indexer>(
		    data_.data() + Indexer::root_indexer::template at<false>(idx) * Indexer::slice_indexer::size);
	}

	template <typename Arg, std::enable_if_t<detail::has_root_indexer<Indexer, Arg>::value, int> = 0>
	constexpr auto slice(Arg&& idx) const
	{
		return indexed_span<Value const, typename Indexer::slice_indexer>(
		    data_.data() + Indexer::root_indexer::template at<false>(idx) * Indexer::slice_indexer::size);
	}

	template <typename Arg, std::enable_if_t<detail::has_root_indexer<Indexer, Arg>::value, int> = 0>
	constexpr auto slice_at(Arg&& idx)
	{
		return indexed_span<Value, typename Indexer::slice_indexer>(
		    data_.data() + Indexer::root_indexer::template at<true>(idx) * Indexer::slice_indexer::size);
	}

	template <typename Arg, std::enable_if_t<detail::has_root_indexer<Indexer, Arg>::value, int> = 0>
	constexpr auto slice_at(Arg&& idx) const
	{
		return indexed_span<Value const, typename Indexer::slice_indexer>(
		    data_.data() + Indexer::root_indexer::template at<true>(idx) * Indexer::slice_indexer::size);
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	static constexpr bool in_range(Args&&... args)
	{
		return Indexer::in_range(std::forward<Args>(args)...);
	}

	static constexpr bool is_o1 = Indexer::is_o1;

	// standard array operations

	/**
	 * @brief Returns an iterator to the beginning of the array. Will return end()
	 * if the array is empty.
	 */
	constexpr iterator begin()
	{
		return data_.begin();
	}

	constexpr const_iterator begin() const
	{
		return data_.begin();
	}

	constexpr const_iterator cbegin() const
	{
		return data_.cbegin();
	}

	constexpr iterator end()
	{
		return data_.end();
	}

	constexpr const_iterator end() const
	{
		return data_.end();
	}

	constexpr const_iterator cend() const
	{
		return data_.cend();
	}

	constexpr reverse_iterator rbegin()
	{
		return data_.rbegin();
	}

	constexpr const_reverse_iterator rbegin() const
	{
		return data_.rbegin();
	}

	constexpr const_reverse_iterator crbegin() const
	{
		return data_.crbegin();
	}

	constexpr reverse_iterator rend()
	{
		return data_.rend();
	}

	constexpr const_reverse_iterator rend() const
	{
		return data_.rend();
	}

	constexpr const_reverse_iterator crend() const
	{
		return data_.crend();
	}

	constexpr reference front()
	{
		return data_.front();
	}

	constexpr const_reference front() const
	{
		return data_.front();
	}

	constexpr reference back()
	{
		return data_.back();
	}
	constexpr const_reference back() const
	{
		return data_.back();
	}

	constexpr size_type size() const
	{
		return Indexer::size;
	}

	constexpr bool empty() const
	{
		return size() == 0;
	}

	constexpr size_type max_size() const
	{
		return size();
	}

	void fill(value_type const& value)
	{
		data_.fill(value);
	}

	void swap(indexed_array& other) noexcept(std::is_nothrow_swappable_v<decltype(data_)>)
	{
		data_.swap(other.data_);
	}

	constexpr value_type* data() noexcept
	{
		return data_.data();
	}

	constexpr value_type const* data() const noexcept
	{
		return data_.data();
	}

  private:
	template <typename V, typename I>
	friend bool operator!=(indexed_array<V, I> const&, indexed_array<V, I> const&);
	template <typename V, typename I>
	friend bool operator<(indexed_array<V, I> const&, indexed_array<V, I> const&);
	template <typename V, typename I>
	friend bool operator>(indexed_array<V, I> const&, indexed_array<V, I> const&);
	template <typename V, typename I>
	friend bool operator>=(indexed_array<V, I> const&, indexed_array<V, I> const&);
	template <typename V, typename I>
	friend bool operator<=(indexed_array<V, I> const&, indexed_array<V, I> const&);
};

//
// Standard array operations
template <typename Value, typename Indexer>
bool operator==(indexed_array<Value, Indexer> const& first, indexed_array<Value, Indexer> const& second)
{
	return !(first != second);
}
template <typename Value, typename Indexer>
bool operator!=(indexed_array<Value, Indexer> const& first, indexed_array<Value, Indexer> const& second)
{
	return first.data_ != second.data_;
}
template <typename Value, typename Indexer>
bool operator<=(indexed_array<Value, Indexer> const& first, indexed_array<Value, Indexer> const& second)
{
	return first.data_ <= second.data_;
}
template <typename Value, typename Indexer>
bool operator>=(indexed_array<Value, Indexer> const& first, indexed_array<Value, Indexer> const& second)
{
	return first.data_ >= second.data_;
}
template <typename Value, typename Indexer>
bool operator<(indexed_array<Value, Indexer> const& first, indexed_array<Value, Indexer> const& second)
{
	return first.data_ < second.data_;
}
template <typename Value, typename Indexer>
bool operator>(indexed_array<Value, Indexer> const& first, indexed_array<Value, Indexer> const& second)
{
	return first.data_ > second.data_;
}

// ADL-versions for at. Supports only single argument
template <typename Value, typename Indexer>
constexpr decltype(auto) at(indexed_array<Value, Indexer>& arr, typename Indexer::index index)
{
	return arr.at(index);
}
template <typename Value, typename Indexer>
constexpr decltype(auto) at(indexed_array<Value, Indexer> const& arr, typename Indexer::index index)
{
	return arr.at(index);
}

} // namespace jbc::indexed_array::detail

namespace std
{
// specialization of tuple_size for all indexed_array-s
template <typename... Args>
struct tuple_size<jbc::indexed_array::detail::indexed_array<Args...>> :
    std::integral_constant<std::size_t, jbc::indexed_array::detail::indexed_array<Args...>::indexer::size>
{
};

} // namespace std

#endif // JBC_INDEXED_ARRAY_DETAIL_INDEXED_ARRAY_H
