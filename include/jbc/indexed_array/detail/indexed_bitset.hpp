//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_INDEXED_BITSET_H
#define JBC_INDEXED_ARRAY_DETAIL_INDEXED_BITSET_H

#include "checked_arg.hpp"
#include "indexed_array.hpp"

#include <boost/mp11.hpp>

#include <bitset>
#include <functional>
#include <type_traits>

namespace jbc::indexed_array::detail
{

// helper class to provide [] and at() members with correct signature
// specialized later for multidimensional bitsets
template <typename Indexer, typename Owner, typename Index>
class indexed_bitset_helper
{
  public:
	constexpr indexed_bitset_helper() noexcept = default;
	~indexed_bitset_helper() noexcept = default;
	constexpr indexed_bitset_helper(indexed_bitset_helper&&) noexcept = default;
	constexpr indexed_bitset_helper(indexed_bitset_helper const&) noexcept = default;
	constexpr indexed_bitset_helper& operator=(indexed_bitset_helper&&) = default;
	constexpr indexed_bitset_helper& operator=(indexed_bitset_helper const&) = default;

  public:
	using value_type = bool;
	using reference = typename std::bitset<Indexer::size>::reference;
	using const_reference = bool;
	using indexer = Indexer;
	constexpr bool test(Index index) const;
	constexpr reference operator[](Index idx);
	constexpr const_reference operator[](Index idx) const;
	constexpr Owner& set(Index index);
	constexpr Owner& reset(Index index);
	constexpr Owner& flip(Index index);

	static constexpr bool in_range(Index idx)
	{
		return Indexer::in_range(idx);
	}
};

template <typename Indexer>
class indexed_bitset :
    private indexed_bitset_helper<Indexer, indexed_bitset<Indexer>, typename Indexer::index>
{
	template <typename I, typename Owner, typename Idx>
	friend class indexed_bitset_helper;

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

	constexpr indexed_bitset& operator=(indexed_bitset<Indexer> const& other) = default;

	constexpr indexed_bitset& operator=(indexed_bitset<Indexer>&& other) = default;

	constexpr explicit operator std::bitset<Indexer::size> const& () const
	{
		return data_;
	}

	constexpr explicit operator std::bitset<Indexer::size> & ()
	{
		return data_;
	}

	// standard constructor
	template <typename Arg,
	          typename... Args,
	          std::enable_if_t<!has_member_index<boost::mp11::mp_first<boost::mp11::mp_list<Arg, Args...> > >::value &&
	                               !std::is_invocable_v<indexed_bitset(indexed_bitset const&), Arg&&, Args&&...>,
	                           bool> = true>
	constexpr explicit indexed_bitset(Arg&& head, Args&&... list) noexcept :
	    data_{std::forward<Arg>(head), std::forward<Args>(list)...}
	{
	}

	// safe_arg constructor
	template <typename Arg,
	          typename... Args,
	          std::enable_if_t<has_member_index<boost::mp11::mp_first<boost::mp11::mp_list<Arg, Args...> > >::value,
	                           bool> = true>
	constexpr explicit indexed_bitset(Arg&& head, Args&&... args) noexcept :
	    data_{static_cast<bool>(head), static_cast<bool>(args)...}
	{
		static_assert(
		    detail::correct_index<Indexer, typename Arg::checked_arg_index, typename Args::checked_arg_index...>(),
		    "Argument mismatch");
	}

	// at and [] operators. Use inheritance to get correct signature
	using indexed_bitset_helper<Indexer, indexed_bitset<Indexer>, typename Indexer::index>::test;
	using indexed_bitset_helper<Indexer, indexed_bitset<Indexer>, typename Indexer::index>::set;
	using indexed_bitset_helper<Indexer, indexed_bitset<Indexer>, typename Indexer::index>::reset;
	using indexed_bitset_helper<Indexer, indexed_bitset<Indexer>, typename Indexer::index>::flip;
	using indexed_bitset_helper<Indexer, indexed_bitset<Indexer>, typename Indexer::index>::operator[];
	using indexed_bitset_helper<Indexer, indexed_bitset<Indexer>, typename Indexer::index>::in_range;

	static constexpr bool is_o1 = Indexer::is_o1;

	constexpr size_type size() const
	{
		return Indexer::size;
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

//
// Standard array operations
//

// Implementation of the at() function for indexed_array_helper when index is an integral type
// (single dimensional array)
template <typename Indexer, typename Owner, typename Index>
constexpr bool indexed_bitset_helper<Indexer, Owner, Index>::test(Index index) const
{
	return static_cast<Owner const&>(*this).data_.test(indexer::template at<false>(index));
}

template <typename Indexer, typename Owner, typename Index>
constexpr Owner& indexed_bitset_helper<Indexer, Owner, Index>::set(Index index)
{
	static_cast<Owner&>(*this).data_.set(indexer::template at<false>(index));
	return static_cast<Owner&>(*this);
}

template <typename Indexer, typename Owner, typename Index>
constexpr Owner& indexed_bitset_helper<Indexer, Owner, Index>::reset(Index index)
{
	static_cast<Owner&>(*this).data_.reset(indexer::template at<false>(index));
	return static_cast<Owner&>(*this);
}

template <typename Indexer, typename Owner, typename Index>
constexpr Owner& indexed_bitset_helper<Indexer, Owner, Index>::flip(Index index)
{
	static_cast<Owner&>(*this).data_.flip(indexer::template at<false>(index));
	return static_cast<Owner&>(*this);
}

template <typename Indexer, typename Owner, typename Index>
constexpr auto indexed_bitset_helper<Indexer, Owner, Index>::operator[](Index idx) -> reference
{
	static_cast<Owner&>(*this).data_[Indexer::template at<false>(idx)];
	return static_cast<Owner&>(*this);
}

template <typename Indexer, typename Owner, typename Index>
constexpr auto indexed_bitset_helper<Indexer, Owner, Index>::operator[](Index idx) const -> const_reference
{
	static_cast<Owner const&>(*this).data_[Indexer::template at<false>(idx)];
	return static_cast<Owner const&>(*this);
}

//
// Specialisation for the case where index is a typelist
// (multi-dimensional array)
template <typename Indexer, typename Owner, typename... Args>
class indexed_bitset_helper<Indexer, Owner, boost::mp11::mp_list<Args...> >
{
  protected:
	constexpr indexed_bitset_helper() noexcept = default;
	~indexed_bitset_helper() noexcept = default;
	constexpr indexed_bitset_helper(indexed_bitset_helper&&) noexcept = default;
	constexpr indexed_bitset_helper(indexed_bitset_helper const&) noexcept = default;
	constexpr indexed_bitset_helper& operator=(indexed_bitset_helper&&) = default;
	constexpr indexed_bitset_helper& operator=(indexed_bitset_helper const&) = default;

  public:
	using value_type = bool;
	using reference = typename std::bitset<Indexer::size>::reference;
	using const_reference = bool;
	using indexer = Indexer;
	constexpr bool test(Args... index)
	{
		return static_cast<Owner&>(*this).data_.test(indexer::template at<true>(std::forward<Args>(index)...));
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

	static constexpr bool in_range(Args... index)
	{
		return Indexer::in_range(std::forward<Args>(index)...);
	}
	constexpr Owner& set(Args... index)
	{
		static_cast<Owner&>(*this).data_.set(indexer::template at<false>(std::forward<Args>(index)...));
		return static_cast<Owner&>(*this);
	}
	constexpr Owner& reset(Args... index)
	{
		static_cast<Owner&>(*this).data_.reset(indexer::template at<false>(std::forward<Args>(index)...));
		return static_cast<Owner&>(*this);
	}
	constexpr Owner& flip(Args... index)
	{
		static_cast<Owner&>(*this).data_.flip(indexer::template at<false>(std::forward<Args>(index)...));
		return static_cast<Owner&>(*this);
	}
};

} // namespace jbc::indexed_bitset::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_INDEXED_ARRAY_H
