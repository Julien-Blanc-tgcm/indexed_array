//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_DEFAULT_INDEXER_H
#define JBC_INDEXED_ARRAY_DETAIL_DEFAULT_INDEXER_H

#include "is_contiguous.hpp"
#include "is_indexer.hpp"

#include <stdexcept>
#include <type_traits>

#include <boost/config.hpp> // for BOOST_UNLIKELY
#include <boost/describe.hpp>
#include <boost/mp11.hpp>

namespace jbc::indexed_array::detail
{
namespace describe = boost::describe;
namespace mp11 = boost::mp11;

template <class T>
struct type_identity
{
	using type = T;
};

template <class T>
using type_identity_t = typename type_identity<T>::type;

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

template <typename T, T min, T max>
struct default_indexer<interval<min, max>,
                       typename std::enable_if_t<std::is_integral<T>::value || std::is_enum<T>::value, void> >
{
	using integral_index_type = decltype(integral_value_v<T{}>);
	static inline constexpr auto const size = integral_value_v<max> - integral_value_v<min> + 1;
	using index = T;
	template <bool throws_on_error = true>
	static constexpr auto at(index v) noexcept(!throws_on_error)
	{
		if constexpr (throws_on_error)
		{
			if (BOOST_UNLIKELY(static_cast<integral_index_type>(v) < integral_value_v<min> ||
			                   static_cast<integral_index_type>(v) > integral_value_v<max>))
				throw std::out_of_range("Invalid index");
		}
		return (static_cast<integral_index_type>(v) - integral_value_v<min>);
	}
};

template <typename T, T... vals>
struct default_indexer<
    std::integer_sequence<T, vals...>,
    typename std::enable_if_t<detail::is_contiguous_sequence<mp11::mp_list_c<T, vals...> >::value, void> >
{
	static inline constexpr auto const size = integral_value_v<mp11::mp_back<mp11::mp_list_c<T, vals...> >::value> -
	                                          integral_value_v<mp11::mp_front<mp11::mp_list_c<T, vals...> >::value> + 1;

	using index = T;

	template <bool throws_on_error = false>
	static constexpr auto at(index i) noexcept(!throws_on_error)
	{
		if constexpr (throws_on_error)
		{
			if (static_cast<decltype(integral_value<T, T{}>::value)>(i) <
			    integral_value_v<mp11::mp_front<mp11::mp_list_c<T, vals...> >::value>)
				throw std::out_of_range("Invalid index");
			if (static_cast<decltype(integral_value<T, T{}>::value)>(i) >
			    integral_value_v<mp11::mp_back<mp11::mp_list_c<T, vals...> >::value>)
				throw std::out_of_range("Invalid index");
		}
		return static_cast<decltype(integral_value_v<T{}>)>(i) -
		       integral_value_v<mp11::mp_front<mp11::mp_list_c<T, vals...> >::value>;
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

template <typename... Args>
struct describe_to_integer_sequence
{
};

template <typename Enum, template <class...> typename L, typename... Args>
struct describe_to_integer_sequence<Enum, L<Args...> >
{
	using type = std::integer_sequence<Enum, Args::value...>;
};

template <typename Enum>
struct default_indexer<Enum, typename std::enable_if_t<boost::describe::has_describe_enumerators<Enum>::value, void> >
{
	using helper_list_type = typename describe_to_integer_sequence<Enum, describe::describe_enumerators<Enum> >::type;
	using index = Enum;
	static inline constexpr auto const size = default_indexer<helper_list_type>::size;
	template <bool throws_on_error = false>
	static constexpr auto at(Enum v) noexcept(!throws_on_error)
	{
		return default_indexer<helper_list_type>::template at<throws_on_error>(v);
	}
};

template <auto Arg, auto... Args>
struct product
{
	static constexpr const auto value = Arg * product<Args...>::value;
};

template <auto Arg>
struct product<Arg>
{
	static constexpr const auto value = Arg;
};

template <auto... Args>
inline constexpr const auto product_v = product<Args...>::value;

template <typename Arg, typename... Args>
struct at_computation_helper
{
	template <bool c>
	static constexpr auto at(typename Arg::index idx, typename Args::index... rem)
	{
		return Arg::template at<c>(idx) * product_v<Args::size...> +
		       at_computation_helper<Args...>::template at<c>(rem...);
	}
};
template <typename Arg>
struct at_computation_helper<Arg>
{
	template <bool c>
	static constexpr auto at(typename Arg::index idx)
	{
		return Arg::template at<c>(idx);
	}
};

template <typename T, typename = void>
struct add_default_handler_if_needed
{
	using type = default_indexer<T>;
};
template <typename T>
struct add_default_handler_if_needed<T, std::enable_if_t<is_indexer<T>::value, void> >
{
	using type = T;
};

template <typename T>
using add_default_handler_if_needed_t = typename add_default_handler_if_needed<T>::type;

template <typename... Args>
struct to_single_indexer
{
	using type = default_indexer<boost::mp11::mp_list<add_default_handler_if_needed_t<Args>...> >;
};

template <typename Arg>
struct to_single_indexer<Arg>
{
	using type = add_default_handler_if_needed_t<Arg>;
};

template <typename... Args>
using to_single_indexer_t = typename to_single_indexer<Args...>::type;

template <typename Arg, typename... Args>
struct default_indexer<
    boost::mp11::mp_list<Arg, Args...>,
    typename std::enable_if_t<boost::mp11::mp_all<is_indexer<Arg>, is_indexer<Args>...>::value, void> >
{
	using index = boost::mp11::mp_list<typename Arg::index, typename Args::index...>;
	static inline constexpr auto const size = product_v<Arg::size, Args::size...>;

	template <bool throws_on_error = false>
	static constexpr auto at(typename Arg::index arg, typename Args::index... args) noexcept
	{
		return at_computation_helper<Arg, Args...>::template at<throws_on_error>(arg, args...);
	}

	using slice_indexer = to_single_indexer_t<Args...>;
	using root_indexer = Arg;
};

template <typename Arg>
struct wrong_indexer
{
	static_assert(std::is_same_v<Arg, std::true_type>, "There is a problem with the indexer");
};

template <typename Arg>
struct add_default_indexer
{
	using type = default_indexer<Arg>;
};

template <typename Arg>
struct add_default_indexer<default_indexer<Arg> >
{
	using type = default_indexer<Arg>;
};

template <typename Arg>
using add_default_indexer_t = typename add_default_indexer<Arg>::type;

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_DEFAULT_INDEXER_H
