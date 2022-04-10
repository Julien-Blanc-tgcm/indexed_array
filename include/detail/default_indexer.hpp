//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_DETAIL_DEFAULT_INDEXER_H
#define JBC_DETAIL_DEFAULT_INDEXER_H

#include "is_contiguous.hpp"

#include <stdexcept>
#include <type_traits>

#include <boost/describe.hpp>
#include <boost/mp11.hpp>

namespace jbc::indexed_array::detail
{
using namespace std;
namespace describe = boost::describe;
namespace mp11 = boost::mp11;

template <class T>
struct type_identity
{
	using type = T;
};

template <class T>
using type_identity_t = typename type_identity<T>::type;

template <typename T, typename T2 = void>
struct has_member_size : public std::false_type
{
};

template <typename T>
struct has_member_size<T, std::enable_if_t<std::is_integral<decltype(T::size)>::value, void> > : public std::true_type
{
};

template <auto minInclusive, type_identity_t<decltype(minInclusive)> maxInclusive>
struct interval
{
	using type = decltype(minInclusive);
	static inline constexpr type const min = minInclusive;
	static inline constexpr type const max = maxInclusive;
};

template <typename index, typename T = void>
struct default_indexer
{
};

template <typename Enum>
struct default_indexer<
    Enum,
    typename std::enable_if_t<boost::describe::has_describe_enumerators<Enum>::value &&
                                  !detail::is_contiguous_sequence<describe::describe_enumerators<Enum> >::value,
                              void> >
{
	static inline constexpr auto const size = mp11::mp_size<describe::describe_enumerators<Enum> >::value;

	template <bool throws_on_error = false>
	static constexpr auto at(Enum v) noexcept(!throws_on_error)
	{
		std::size_t ret = 0;
		bool found = false;
		mp11::mp_for_each<describe::describe_enumerators<Enum> >([v, &ret, &found](auto I) {
			if (I.value == v)
			{
				found = true;
			}
			if (!found)
				++ret;
		});
		if (!found)
		{
			if constexpr (throws_on_error)
				throw std::out_of_range{"Invalid index"};
			return std::numeric_limits<std::size_t>::max(); // return an invalid value
		}
		return ret;
	}
	using index = Enum;
};

template <typename Enum>
struct default_indexer<
    Enum,
    typename std::enable_if_t<boost::describe::has_describe_enumerators<Enum>::value &&
                                  detail::is_contiguous_sequence<boost::describe::describe_enumerators<Enum> >::value,
                              void> >
{
	static inline constexpr auto const size = static_cast<typename std::underlying_type<Enum>::type>(
	                                              mp11::mp_back<boost::describe::describe_enumerators<Enum> >::value) -
	                                          static_cast<typename std::underlying_type<Enum>::type>(
	                                              mp11::mp_front<boost::describe::describe_enumerators<Enum> >::value) +
	                                          1;

	template <bool throws_on_error = false>
	static constexpr auto at(Enum v) noexcept(!throws_on_error)
	{
		if constexpr (throws_on_error)
		{
			if (static_cast<std::underlying_type_t<Enum> >(v) <
			    static_cast<underlying_type_t<Enum> >(
			        mp11::mp_front<boost::describe::describe_enumerators<Enum> >::value))
				throw std::out_of_range("Invalid index");
			if (static_cast<std::underlying_type_t<Enum> >(v) >
			    static_cast<underlying_type_t<Enum> >(
			        mp11::mp_back<boost::describe::describe_enumerators<Enum> >::value))
				throw std::out_of_range("Invalid index");
		}
		return static_cast<std::underlying_type_t<Enum> >(v) -
		       static_cast<underlying_type_t<Enum> >(
		           mp11::mp_front<boost::describe::describe_enumerators<Enum> >::value);
	}
	using index = Enum;
};

template <typename T, T... vals>
struct default_indexer<
    std::integer_sequence<T, vals...>,
    typename std::enable_if_t<detail::is_contiguous_sequence<mp11::mp_list_c<T, vals...> >::value, void> >
{
	static inline constexpr auto const size = static_cast<T>(mp11::mp_back<mp11::mp_list_c<T, vals...> >::value) -
	                                          static_cast<T>(mp11::mp_front<mp11::mp_list_c<T, vals...> >::value) + 1;

	using index = T;

	template <bool throws_on_error = false>
	static constexpr auto at(index i) noexcept(!throws_on_error)
	{
		if constexpr (throws_on_error)
		{
			if (i < mp11::mp_front<mp11::mp_list_c<T, vals...> >::value)
				throw std::out_of_range("Invalid index");
			if (i > mp11::mp_back<mp11::mp_list_c<T, vals...> >::value)
				throw std::out_of_range("Invalid index");
		}
		return i - mp11::mp_front<mp11::mp_list_c<T, vals...> >::value;
	}
};

template <typename T, T... vals>
struct default_indexer<
    std::integer_sequence<T, vals...>,
    typename std::enable_if_t<!detail::is_contiguous_sequence<mp11::mp_list_c<T, vals...> >::value, void> >
{
	static inline constexpr auto size = mp11::mp_size<mp11::mp_unique<mp11::mp_list_c<T, vals...> > >::value;

	using index = T;

	template <bool throws_on_error = false>
	static constexpr auto at(index v)
	{
		std::size_t ret = 0;
		bool found = false;
		mp11::mp_for_each<mp11::mp_unique<mp11::mp_list_c<T, vals...> > >([v, &ret, &found](auto I) {
			if (I.value == v)
			{
				found = true;
			}
			if (!found)
				++ret;
		});
		if (!found)
		{
			if constexpr (throws_on_error)
				throw std::out_of_range("Invalid index");
			return std::numeric_limits<std::size_t>::max(); // return an invalid value
		}
		return ret;
	}
};

template <typename T, T min, T max>
struct default_indexer<interval<min, max>, typename std::enable_if_t<std::is_integral<T>::value, void> >
{
	static inline constexpr auto const size = max - min + 1;
	using index = T;
	template <bool throws_on_error = true>
	static constexpr auto at(index v) noexcept(!throws_on_error)
	{
		if constexpr (throws_on_error)
		{
			if (v < min || v > max)
				throw std::out_of_range("Invalid index");
		}
		return (v - min);
	}
};

template <typename T, T min, T max>
struct default_indexer<interval<min, max>, typename std::enable_if_t<std::is_enum<T>::value, void> >
{
	static inline constexpr auto const size =
	    static_cast<std::underlying_type_t<T> >(max) - static_cast<std::underlying_type_t<T> >(min) + 1;
	using index = T;
	template <bool throws_on_error = false>
	static constexpr auto at(index v) noexcept(!throws_on_error)
	{
		if constexpr (throws_on_error)
		{
			if (static_cast<std::underlying_type_t<T> >(v) < static_cast<std::underlying_type_t<T> >(min) ||
			    static_cast<std::underlying_type_t<T> >(v) > static_cast<std::underlying_type_t<T> >(max))
				throw std::out_of_range("Invalid index");
		}
		return (static_cast<std::underlying_type_t<T> >(v) - static_cast<std::underlying_type_t<T> >(min));
	}
};

} // namespace jbc::indexed_array::detail

#endif // JBC_DETAIL_DEFAULT_INDEXER_H
