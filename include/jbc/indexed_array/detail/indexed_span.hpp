// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_SPAN_DETAIL_INDEXED_SPAN_H
#define JBC_INDEXED_SPAN_DETAIL_INDEXED_SPAN_H

#include "default_indexer.hpp"
#include <boost/mp11.hpp>

#include <functional>
#include <type_traits>

namespace jbc::indexed_array::detail
{

template <typename Value, typename Indexer>
class indexed_span
{
	template <typename V, typename I, typename Owner, typename Idx>
	friend class indexed_span_helper;

	Value* data_{nullptr};

  public:
	// standard array members
	using value_type = Value;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = value_type const&;
	using pointer = value_type*;
	using const_pointer = value_type const*;

	// specific
	using indexer = Indexer;

	indexed_span() noexcept = delete;

	~indexed_span() noexcept = default;

	// standard constructor
	constexpr indexed_span(Value* data) : data_{data}
	{
	}

	constexpr indexed_span(indexed_span const& other) = default;

	constexpr indexed_span& operator=(indexed_span const& other) = default;

	// at and [] operators. Use enable_if to disable overloads that won't work
	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr reference at(Args&&... args) const
	{
		return data_[Indexer::template at<true>(std::forward<Args>(args)...)];
	}

#if defined(__cpp_multidimensional_subscript)
	// define multi dimension operator[] if available
	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr reference operator[](Args&&... args) const
	{
		auto i = indexer::template at<false>(std::forward<Args>(args)...);
		return data_[i];
	}
#else
	// define standard single dimension operator[] otherwise
	template <typename Arg>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Arg>
	constexpr reference operator[](Arg&& arg) const
	{
		return data_[Indexer::template at<false>(std::forward<Arg>(arg))];
	}
#endif

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	constexpr reference operator[](std::tuple<Args...> arg) const
	{
		auto f = static_cast<std::size_t (*)(std::decay_t<Args>...)>(Indexer::template at<false>);
		auto i = std::apply(f, arg);
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
	constexpr auto slice(Arg idx) const
	{
		return indexed_span<Value const, typename Indexer::slice_indexer>(
		    data_ + Indexer::root_indexer::template at<false>(idx) * Indexer::slice_indexer::size);
	}

	template <typename Arg, std::enable_if_t<detail::has_root_indexer<Indexer, Arg>::value, int> = 0>
	constexpr auto slice_at(Arg idx) const
	{
		return indexed_span<Value const, typename Indexer::slice_indexer>(
		    data_ + Indexer::root_indexer::template at<true>(idx) * Indexer::slice_indexer::size);
	}

	template <typename... Args>
	    requires jbc::indexed_array::concepts::indexer_invocable_with<indexer, Args...>
	static constexpr bool in_range(Args&&... args)
	{
		return Indexer::in_range(std::forward<Args>(args)...);
	}

	using iterator = Value*;
	using const_iterator = Value const*;
	using reverse_iterator = std::reverse_iterator<Value*>;
	using const_reverse_iterator = std::reverse_iterator<Value const*>;

	static constexpr bool is_o1 = Indexer::is_o1;

	// standard span operations
	constexpr iterator begin() const
	{
		return data_;
	}

	constexpr const_iterator cbegin() const
	{
		return begin();
	}

	constexpr iterator end() const
	{
		return data_ + Indexer::size;
	}

	constexpr const_iterator cend() const
	{
		return end();
	}

	constexpr reverse_iterator rbegin() const
	{
		return std::make_reverse_iterator(end());
	}

	constexpr const_reverse_iterator crbegin() const
	{
		return rbegin();
	}

	constexpr reverse_iterator rend() const
	{
		return std::make_reverse_iterator(data_);
	}

	constexpr const_reverse_iterator crend() const
	{
		return rend();
	}

	constexpr reference front() const
	{
		return *data_;
	}
	constexpr reference back() const
	{
		return *(data_ + Indexer::size - 1);
	}

	constexpr size_type size() const
	{
		return Indexer::size;
	}
	constexpr bool empty() const
	{
		return size() == 0;
	}

	constexpr pointer data() const
	{
		return data_;
	}
};

// ADL-versions for at. Supports only single argument
template <typename Value, typename Indexer>
constexpr decltype(auto) at(indexed_span<Value, Indexer>& arr, typename Indexer::index index)
{
	return arr.at(index);
}
template <typename Value, typename Indexer>
constexpr decltype(auto) at(indexed_span<Value, Indexer> const& arr, typename Indexer::index index)
{
	return arr.at(index);
}

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_SPAN_DETAIL_INDEXED_SPAN_H
