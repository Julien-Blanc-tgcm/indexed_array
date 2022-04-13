//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_DETAIL_CHECKED_ARG_H
#define JBC_DETAIL_CHECKED_ARG_H

#include <boost/mp11.hpp>

#include <cstdint>
#include <type_traits>
#include <utility>

namespace jbc::indexed_array::detail
{

template <typename Indexer, typename C>
struct at_helper
{
	static constexpr auto at()
	{
		return Indexer::at(C::value);
	}
};

template <typename Indexer, template <typename...> class L, typename... Args>
struct at_helper<Indexer, L<Args...> >
{
	static constexpr auto at()
	{
		return Indexer::template at(Args::value...);
	}
};

template <typename C, typename T>
struct checked_arg
{
	using index = C;
	using value_type = T;
	T&& v;
	explicit checked_arg(T&& val) : v{std::forward<T>(val)} {};
	explicit operator T&&()
	{
		return std::forward<T>(v);
	}
};

template <typename C, typename T>
struct checked_arg_h
{
	using index = C;
	using value_type = T;
	T&& v;
	explicit checked_arg_h(T&& val) : v{std::forward<T>(val)} {};
	operator T&&()
	{
		return std::forward<T>(v);
	}
	template <typename U = T, typename = typename std::enable_if_t<std::is_array<U>::value> >
	operator checked_arg<C, std::decay_t<T>*>()
	{
		return checked_arg<C, std::decay_t<T>*>(v);
	}
	operator checked_arg<C, T>()
	{
		return checked_arg<C, T>(v);
	}
};

template <template <auto...> class L, typename T, auto... C>
struct checked_arg_h<L<C...>, T>
{
	using index = boost::mp11::mp_list<std::integral_constant<decltype(C), C>...>;
	using value_type = T;
	T&& v;
	explicit checked_arg_h(T&& val) : v{std::forward<T>(val)} {};
	operator T&&()
	{
		return std::forward<T>(v);
	}
	template <typename U = T, typename = typename std::enable_if_t<std::is_array<U>::value> >
	operator checked_arg<index, std::decay_t<T>*>()
	{
		return checked_arg<index, std::decay_t<T>*>(v);
	}
	operator checked_arg<index, T>()
	{
		return checked_arg<index, T>(v);
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

template <typename Indexer, typename... v>
constexpr bool correct_index()
{
	return correct_index_<Indexer, 0, v...>();
}

} // namespace jbc::indexed_array::detail

#endif // JBC_DETAIL_CHECKED_ARG_H
