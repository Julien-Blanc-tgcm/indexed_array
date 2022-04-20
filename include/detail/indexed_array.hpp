//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_INDEXED_ARRAY_H
#define JBC_INDEXED_ARRAY_DETAIL_INDEXED_ARRAY_H

#include "checked_arg.hpp"
#include "indexed_span.hpp"

#include <boost/mp11.hpp>

#include <array>
#include <functional>
#include <type_traits>

namespace jbc::indexed_array::detail
{
template <typename T, typename T2 = void>
struct has_member_index : public std::false_type
{
};

template <typename T>
struct has_member_index<T, std::enable_if_t<std::is_class<typename T::checked_arg_index>::value, void> > :
    public std::true_type
{
};

// helper class to provide [] and at() members with correct signature
// specialized later for multidimensional arrays
template <typename Value, typename Indexer, typename Owner, typename Index>
class indexed_array_helper
{
  protected:
	indexed_array_helper() noexcept = default;
	~indexed_array_helper() noexcept = default;
	indexed_array_helper(indexed_array_helper&&) noexcept = default;
	indexed_array_helper(indexed_array_helper const&) noexcept = default;
	indexed_array_helper& operator=(indexed_array_helper&&) = default;
	indexed_array_helper& operator=(indexed_array_helper const&) = default;

  public:
	using value_type = Value;
	using reference = value_type&;
	using const_reference = value_type const&;
	using indexer = Indexer;
	constexpr reference at(Index index);
	constexpr const_reference at(Index index) const;
	constexpr reference operator[](Index idx);
	constexpr const_reference operator[](Index idx) const;
};

template <typename Value, typename Indexer>
class indexed_array :
    public indexed_array_helper<Value, Indexer, indexed_array<Value, Indexer>, typename Indexer::index>
{
	template <typename V, typename I, typename Owner, typename Idx>
	friend class indexed_array_helper;

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

	// standard constructor
	template <
	    typename... Args,
	    std::enable_if_t<!has_member_index<boost::mp11::mp_first<boost::mp11::mp_list<Args...> > >::value, bool> = true>
	constexpr indexed_array(Args&&... list) : data_{std::forward<Args>(list)...}
	{
	}
	// safe_arg constructor
	template <
	    typename... Args,
	    std::enable_if_t<has_member_index<boost::mp11::mp_first<boost::mp11::mp_list<Args...> > >::value, bool> = true>
	constexpr indexed_array(Args&&... args) : data_{static_cast<Value>(args)...}
	{
		static_assert(detail::correct_index<Indexer, typename Args::checked_arg_index...>(), "Argument mismatch");
	}

	constexpr indexed_array(indexed_array const& other) = default;

	constexpr indexed_array(indexed_array&& other) = default;

	constexpr indexed_array& operator=(indexed_array const& other) = default;

	constexpr indexed_array& operator=(indexed_array&& other) = default;

	// at and [] operators. Use inheritance to get correct signature
	using indexed_array_helper<Value, Indexer, indexed_array<Value, Indexer>, typename Indexer::index>::at;
	using indexed_array_helper<Value, Indexer, indexed_array<Value, Indexer>, typename Indexer::index>::operator[];

	// standard array operations
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

//
// Implementation of the at() function for indexed_array_helper when index is an integral type
// (single dimensional array)
template <typename Value, typename Indexer, typename Owner, typename Index>
constexpr auto indexed_array_helper<Value, Indexer, Owner, Index>::at(Index index) -> reference
{
	return static_cast<Owner&>(*this).data_[indexer::template at<true>(index)];
}
template <typename Value, typename Indexer, typename Owner, typename Index>
constexpr auto indexed_array_helper<Value, Indexer, Owner, Index>::at(Index index) const -> const_reference
{
	return static_cast<Owner const&>(*this).data_[indexer::template at<true>(index)];
}
template <typename Value, typename Indexer, typename Owner, typename Index>
constexpr auto indexed_array_helper<Value, Indexer, Owner, Index>::operator[](Index idx) -> reference
{
	return static_cast<Owner&>(*this).data_[Indexer::template at<false>(idx)];
}

template <typename Value, typename Indexer, typename Owner, typename Index>
constexpr auto indexed_array_helper<Value, Indexer, Owner, Index>::operator[](Index idx) const -> const_reference
{
	return static_cast<Owner const&>(*this).data_[Indexer::template at<false>(idx)];
}

//
// Specialisation for the case where index is a typelist
// (multi-dimensional array)
template <typename Value, typename Indexer, typename Owner, template <class...> class Index, typename... Args>
class indexed_array_helper<Value, Indexer, Owner, Index<Args...> >
{
  protected:
	indexed_array_helper() noexcept = default;
	~indexed_array_helper() noexcept = default;
	indexed_array_helper(indexed_array_helper&&) noexcept = default;
	indexed_array_helper(indexed_array_helper const&) noexcept = default;
	indexed_array_helper& operator=(indexed_array_helper&&) = default;
	indexed_array_helper& operator=(indexed_array_helper const&) = default;

  public:
	using value_type = Value;
	using reference = value_type&;
	using const_reference = value_type const&;
	using indexer = Indexer;
	constexpr reference at(Args... index)
	{
		return static_cast<Owner&>(*this).data_[indexer::template at<true>(std::forward<Args>(index)...)];
	}
	constexpr const_reference at(Args... index) const
	{
		return static_cast<Owner const&>(*this).data_[indexer::template at<true>(std::forward<Args>(index)...)];
	}
	constexpr reference operator[](std::tuple<Args...> idx)
	{
		auto i = std::apply(indexer::template at<false>, idx);
		return static_cast<Owner&>(*this).data_[i];
	}
	constexpr const_reference operator[](std::tuple<Args...> idx) const
	{
		auto i = std::apply(indexer::template at<false>, idx);
		return static_cast<Owner const&>(*this).data_[i];
	}
	constexpr indexed_span<Value, typename Indexer::slice_indexer> slice(typename Indexer::root_indexer::index idx)
	{
		return indexed_span<Value, typename Indexer::slice_indexer>(
		    static_cast<Owner&>(*this).data_.data() + Indexer::root_indexer::at(idx) * Indexer::slice_indexer::size);
	}
	constexpr indexed_span<Value const, typename Indexer::slice_indexer>
	slice(typename Indexer::root_indexer::index idx) const
	{
		return indexed_span<Value const, typename Indexer::slice_indexer>(
		    static_cast<Owner const&>(*this).data_.data() +
		    Indexer::root_indexer::at(idx) * Indexer::slice_indexer::size);
	}
};

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

#endif // JBC_INDEXED_ARRAY_DETAIL_INDEXED_ARRAY_H
