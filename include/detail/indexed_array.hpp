#ifndef JBC_DETAIL_INDEXED_ARRAY_H
#define JBC_DETAIL_INDEXED_ARRAY_H

#include "checked_arg.hpp"

#include <array>
#include <type_traits>

namespace jbc::indexed_array::detail
{

template <typename T, typename T2 = void>
struct has_member_index : public std::false_type
{
};

template <typename T>
struct has_member_index<
    T,
    std::enable_if_t<std::is_integral<decltype(T::index)>::value || std::is_enum<decltype(T::index)>::value, void> > :
    public std::true_type
{
};

template <typename Value, typename Indexer>
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

	// specific
	using indexer = Indexer;

	indexed_array() noexcept = default;

	~indexed_array() noexcept = default;

	template <typename... Args,
	          std::enable_if_t<!has_member_index<mp11::mp_first<mp11::mp_list<Args...> > >::value, bool> = true>
	indexed_array(Args&&... list) : data_{std::forward<Args>(list)...}
	{
	}

	template <typename... Args,
	          std::enable_if_t<has_member_index<mp11::mp_first<mp11::mp_list<Args...> > >::value, bool> = true>
	indexed_array(Args&&... args) : data_{static_cast<Value>(args)...}
	{
		static_assert(detail::correct_index<Indexer, Args::index...>(), "Argument mismatch");
	}

	indexed_array(indexed_array const& other) = default;

	indexed_array(indexed_array&& other) = default;

	indexed_array& operator=(indexed_array const& other) = default;

	indexed_array& operator=(indexed_array&& other) = default;

	constexpr reference operator[](typename Indexer::index idx)
	{
		return data_[Indexer::template at<false>(idx)];
	}

	constexpr const_reference operator[](typename Indexer::index idx) const
	{
		return data_[Indexer::template at<false>(idx)];
	}

	constexpr reference at(typename Indexer::index idx)
	{
		return data_[Indexer::template at<true>(idx)];
	}

	constexpr const_reference at(typename Indexer::index idx) const
	{
		return data_[Indexer::template at<true>(idx)];
	}

	auto begin()
	{
		return data_.begin();
	}
	auto begin() const
	{
		return data_.begin();
	}
	auto cbegin() const
	{
		return data_.cbegin();
	}
	auto end()
	{
		return data_.end();
	}
	auto end() const
	{
		return data_.end();
	}
	auto cend() const
	{
		return data_.cend();
	}
	auto rbegin()
	{
		return data_.rbegin();
	}
	auto rbegin() const
	{
		return data_.rbegin();
	}
	auto crbegin() const
	{
		return data_.crbegin();
	}
	auto rend()
	{
		return data_.rend();
	}
	auto rend() const
	{
		return data_.rend();
	}
	auto crend() const
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

#endif // JBC_DETAIL_INDEXED_ARRAY_H
