//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_SPAN_DETAIL_INDEXED_SPAN_H
#define JBC_INDEXED_SPAN_DETAIL_INDEXED_SPAN_H

#include <boost/mp11.hpp>

#include <functional>
#include <type_traits>

namespace jbc::indexed_array::detail
{
// helper class to provide [] and at() members with correct signature
// specialized later for multidimensional arrays
template <typename Value, typename Indexer, typename Owner, typename Index>
class indexed_span_helper
{
  protected:
	indexed_span_helper() noexcept = default;
	~indexed_span_helper() noexcept = default;
	indexed_span_helper(indexed_span_helper&&) noexcept = default;
	indexed_span_helper(indexed_span_helper const&) noexcept = default;
	indexed_span_helper& operator=(indexed_span_helper&&) = default;
	indexed_span_helper& operator=(indexed_span_helper const&) = default;

  public:
	using value_type = Value;
	using reference = value_type&;
	using const_reference = value_type const&;
	using indexer = Indexer;
	constexpr reference at(Index index);
	constexpr const_reference at(Index index) const;
	constexpr reference operator[](Index idx);
	constexpr const_reference operator[](Index idx) const;
	constexpr value_type slice(Index idx)
	{
		return operator[](idx);
	}
};

template <typename Value, typename Indexer>
class indexed_span : private indexed_span_helper<Value, Indexer, indexed_span<Value, Indexer>, typename Indexer::index>
{
	template <typename V, typename I, typename Owner, typename Idx>
	friend class indexed_span_helper;

	Value* data_;

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

	indexed_span() noexcept = default;

	~indexed_span() noexcept = default;

	// standard constructor
	constexpr indexed_span(Value* data) : data_{data}
	{
	}

	constexpr indexed_span(indexed_span const& other) = default;

	constexpr indexed_span(indexed_span&& other) = default;

	constexpr indexed_span& operator=(indexed_span const& other) = default;

	constexpr indexed_span& operator=(indexed_span&& other) = default;

	// at and [] operators. Use inheritance to get correct signature
	using indexed_span_helper<Value, Indexer, indexed_span<Value, Indexer>, typename Indexer::index>::at;
	using indexed_span_helper<Value, Indexer, indexed_span<Value, Indexer>, typename Indexer::index>::operator[];
	using indexed_span_helper<Value, Indexer, indexed_span<Value, Indexer>, typename Indexer::index>::slice;

	// standard array operations
	auto begin()
	{
		return data_;
	}
	auto begin() const
	{
		return data_;
	}
	auto end()
	{
		return data_ + Indexer::size;
	}
	auto end() const
	{
		return data_ + Indexer::size;
	}
	//	auto rbegin()
	//	{
	//		return data_.rbegin();
	//	}
	//	auto rbegin() const
	//	{
	//		return data_.rbegin();
	//	}
	//	auto rend()
	//	{
	//		return data_.rend();
	//	}
	//	auto rend() const
	//	{
	//		return data_.rend();
	//	}

	constexpr reference front()
	{
		return *data_;
	}
	constexpr const_reference front() const
	{
		return *data_;
	}
	constexpr reference back()
	{
		return *(data_ + Indexer::size - 1);
	}
	constexpr const_reference back() const
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
};

//
// Implementation of the at() function for indexed_span_helper when index is an integral type
// (single dimensional span)
template <typename Value, typename Indexer, typename Owner, typename Index>
constexpr auto indexed_span_helper<Value, Indexer, Owner, Index>::at(Index index) -> reference
{
	return static_cast<Owner&>(*this).data_[indexer::template at<true>(index)];
}
template <typename Value, typename Indexer, typename Owner, typename Index>
constexpr auto indexed_span_helper<Value, Indexer, Owner, Index>::at(Index index) const -> const_reference
{
	return static_cast<Owner const&>(*this).data_[indexer::template at<true>(index)];
}
template <typename Value, typename Indexer, typename Owner, typename Index>
constexpr auto indexed_span_helper<Value, Indexer, Owner, Index>::operator[](Index idx) -> reference
{
	return static_cast<Owner&>(*this).data_[Indexer::template at<false>(idx)];
}

template <typename Value, typename Indexer, typename Owner, typename Index>
constexpr auto indexed_span_helper<Value, Indexer, Owner, Index>::operator[](Index idx) const -> const_reference
{
	return static_cast<Owner const&>(*this).data_[Indexer::template at<false>(idx)];
}

//
// Specialisation for the case where index is a typelist
// (multi-dimensional span)
template <typename Value, typename Indexer, typename Owner, template <class...> class Index, typename... Args>
class indexed_span_helper<Value, Indexer, Owner, Index<Args...> >
{
  protected:
  protected:
	indexed_span_helper() noexcept = default;
	~indexed_span_helper() noexcept = default;
	indexed_span_helper(indexed_span_helper&&) noexcept = default;
	indexed_span_helper(indexed_span_helper const&) noexcept = default;
	indexed_span_helper& operator=(indexed_span_helper&&) = default;
	indexed_span_helper& operator=(indexed_span_helper const&) = default;

  public:
	using value_type = Value;
	using reference = value_type&;
	using const_reference = value_type const&;
	using indexer = Indexer;
	constexpr reference at(Args... index)
	{
		return *(static_cast<Owner&>(*this).data_ + indexer::template at<true>(std::forward<Args>(index)...));
	}
	constexpr const_reference at(Args... index) const
	{
		return *(static_cast<Owner const&>(*this).data_ + indexer::template at<true>(std::forward<Args>(index)...));
	}
	constexpr reference operator[](std::tuple<Args...> idx)
	{
		auto i = std::apply(indexer::template at<false>, idx);
		return *(static_cast<Owner&>(*this).data_ + i);
	}
	constexpr const_reference operator[](std::tuple<Args...> idx) const
	{
		auto i = std::apply(indexer::template at<false>, idx);
		return *(static_cast<Owner const&>(*this).data_ + i);
	}
	constexpr indexed_span<Value, typename Indexer::slice_indexer> slice(typename Indexer::root_indexer::index idx)
	{
		return indexed_span<Value, typename Indexer::slice_indexer>(
		    static_cast<Owner&>(*this).data_ + Indexer::root_indexer::at(idx) * Indexer::slice_indexer::size);
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
