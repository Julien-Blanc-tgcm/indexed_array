//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_DETAIL_CHECKED_ARG_H
#define JBC_DETAIL_CHECKED_ARG_H

#include <cstdint>
#include <type_traits>
#include <utility>

namespace jbc::indexed_array::detail
{

template <auto C, typename T>
struct checked_arg
{
	static constexpr auto const index = C;
	using value_type = T;
	T&& v;
	explicit checked_arg(T&& val) : v{std::forward<T>(val)} {};
	explicit operator T&&()
	{
		return std::forward<T>(v);
	}
};

template <auto C, typename T>
struct checked_arg_h
{
	static constexpr auto const index = C;
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

template <typename Indexer, std::size_t X>
constexpr bool correct_index_()
{
	static_assert(!(X < Indexer::size), "Not enough initializers provided");
	static_assert(!(X > Indexer::size), "Too many initializers provided");
	return X == Indexer::size;
}

template <typename Indexer, std::size_t X, auto u, auto... v>
constexpr bool correct_index_()
{
	static_assert(X == Indexer::at(u), "Invalid value for initializer");
	return X == Indexer::at(u) && correct_index_<Indexer, X + 1, v...>();
}

template <typename Indexer, auto... v>
constexpr bool correct_index()
{
	return correct_index_<Indexer, 0, v...>();
}

} // namespace jbc::indexed_array::detail

#endif // JBC_DETAIL_CHECKED_ARG_H
