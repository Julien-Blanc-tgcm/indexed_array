//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_CHECKED_ARG_H
#define JBC_INDEXED_ARRAY_DETAIL_CHECKED_ARG_H

#include <boost/mp11.hpp>

#include <cstdint>
#include <type_traits>
#include <utility>

namespace jbc::indexed_array::detail
{

template <typename Indexer, typename Index>
struct at_helper
{
	static constexpr auto at()
	{
		return Indexer::template at<true>(Index::value);
	}
};

template <typename Indexer, template <typename...> class L, typename... Args>
struct at_helper<Indexer, L<Args...> >
{
	static constexpr auto at()
	{
		return Indexer::template at<true>(Args::value...);
	}
};

template <typename Index, typename Value>
struct checked_arg
{
	using checked_arg_index = Index;
	using value_type = Value;
	value_type&& v;
	explicit constexpr checked_arg(value_type&& val) : v{std::forward<value_type>(val)} {};
	explicit constexpr operator value_type&&()
	{
		return std::forward<value_type>(v);
	}
};

template <typename Index, typename Value>
struct checked_arg_h
{
	using checked_arg_index = Index;
	using value_type = Value;
	value_type&& v;
	explicit constexpr checked_arg_h(value_type&& val) : v{std::forward<value_type>(val)} {};
	constexpr operator value_type&&()
	{
		return std::forward<value_type>(v);
	}
	constexpr operator checked_arg<checked_arg_index, value_type>()
	{
		return checked_arg<checked_arg_index, value_type>(v);
	}
};

template <template <auto...> class List, typename Value, auto... Indexes>
struct checked_arg_h<List<Indexes...>, Value>
{
	using checked_arg_index = boost::mp11::mp_list<std::integral_constant<decltype(Indexes), Indexes>...>;
	using value_type = Value;
	value_type&& v;
	explicit constexpr checked_arg_h(value_type&& val) : v{std::forward<value_type>(val)} {};
	constexpr operator value_type&&()
	{
		return std::forward<value_type>(v);
	}
	constexpr operator checked_arg<checked_arg_index, value_type>()
	{
		return checked_arg<checked_arg_index, value_type>(v);
	}
};

template <typename Indexer, std::size_t X>
constexpr bool correct_index_()
{
	static_assert(!(X < Indexer::size), "Not enough initializers provided");
	static_assert(!(X > Indexer::size), "Too many initializers provided");
	return X == Indexer::size;
}

template <typename Indexer, std::size_t X, typename u, typename... v>
constexpr bool correct_index_()
{
	static_assert(X == at_helper<Indexer, u>::at(), "Invalid value for initializer");
	return X == at_helper<Indexer, u>::at() && correct_index_<Indexer, X + 1, v...>();
}

template <typename Indexer, typename... Args>
constexpr bool correct_index()
{
	return correct_index_<Indexer, 0, Args...>();
}

template <typename T, typename T2 = void>
struct is_checked_arg : public std::false_type
{
};

template <typename T>
struct is_checked_arg<T, std::enable_if_t<std::is_class<typename T::checked_arg_index>::value, void> > :
    public std::true_type
{
};

template <typename T>
constexpr bool is_checked_arg_v = is_checked_arg<T>::value;

} // namespace jbc::indexed_array::detail

#if __cpp_concepts >= 202002L
namespace jbc::indexed_array::concepts
{
template <class From, class To>
concept explicitly_convertible_to = requires(From f) { static_cast<To>(f); };

template<typename Arg, typename Value>
concept checked_arg = ::jbc::indexed_array::detail::is_checked_arg_v<Arg> && explicitly_convertible_to<Arg, Value>;

} // jbc::indexed_array::concepts
#endif

#endif // JBC_INDEXED_ARRAY_DETAIL_CHECKED_ARG_H
