// ·Copyright·2022·Julien Blanc
// ·Distributed·under·the·Boost·Software·License,·Version·1.0.
// ·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_DEFAULT_INDEXER_H
#define JBC_INDEXED_ARRAY_DETAIL_DEFAULT_INDEXER_H

#include "index_range.hpp"
#include "is_contiguous.hpp"
#include "is_indexer.hpp"
#include "value_sequence.hpp"

#include <stdexcept>
#include <type_traits>
#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
#include <concepts>
#endif

#include <boost/config.hpp> // for BOOST_UNLIKELY
#include <boost/mp11.hpp>

namespace jbc::indexed_array::detail
{
namespace mp11 = boost::mp11;

template <typename index, typename T = void>
struct default_indexer
{
};

template <typename indexer, typename T = void>
struct is_o1_indexer : public std::false_type
{
};

template <typename indexer>
struct is_o1_indexer<indexer, std::enable_if_t<std::is_same_v<decltype(indexer::is_o1), bool const>, void>> :
public std::integral_constant<bool, indexer::is_o1>
{
};

template <typename T, T min, T max>
struct default_indexer<index_range<min, max>, std::enable_if_t<can_be_integral_value<T, min>::value, void> >
{
	using integral_index_type = decltype(integral_value_v<T{}>);
	static inline constexpr auto const size = integral_value_v<max> - integral_value_v<min> + 1;

	static constexpr bool is_o1 = true;

	using index = T;
	template <bool throws_on_error>
	static constexpr std::size_t at(T v) noexcept(!throws_on_error)
	{
		if constexpr (throws_on_error)
		{
			if (BOOST_UNLIKELY(!in_range(v)))
				throw std::out_of_range("Invalid index");
		}
		return std::size_t(static_cast<integral_index_type>(v) - integral_value_v<min>);
	}

	static constexpr auto in_range(T v) noexcept
	{
		return !(static_cast<integral_index_type>(v) < integral_value_v<min> ||
		         static_cast<integral_index_type>(v) > integral_value_v<max>);
	}
};

template <typename T, T... vals>
struct default_indexer<
    value_sequence<T, vals...>,
    typename std::enable_if_t<detail::is_contiguous_sequence<mp11::mp_list_c<T, vals...> >::value, void> >
{
	static inline constexpr auto const size = integral_value_v<mp11::mp_back<mp11::mp_list_c<T, vals...> >::value> -
	                                          integral_value_v<mp11::mp_front<mp11::mp_list_c<T, vals...> >::value> + 1;

	static constexpr bool is_o1 = true;

	using index = T;

	template <bool throws_on_error = false>
	static constexpr std::size_t at(index i) noexcept(!throws_on_error)
	{
		if constexpr (throws_on_error)
		{
			if (!in_range(i))
				throw std::out_of_range("Invalid index");
		}
		return std::size_t(static_cast<decltype(integral_value_v<T{}>)>(i) -
		                   integral_value_v<mp11::mp_front<mp11::mp_list_c<T, vals...> >::value>);
	}

	static constexpr bool in_range(index i)
	{
		return (static_cast<decltype(integral_value<T, T{}>::value)>(i) >=
		        integral_value_v<mp11::mp_front<mp11::mp_list_c<T, vals...> >::
		                             value>)&&(static_cast<decltype(integral_value<T, T{}>::value)>(i) <=
		                                       integral_value_v<mp11::mp_back<mp11::mp_list_c<T, vals...> >::value>);
	}
};

template <typename T, T... vals>
struct default_indexer<
    value_sequence<T, vals...>,
    typename std::enable_if_t<!detail::is_contiguous_sequence<mp11::mp_list_c<T, vals...> >::value, void> >
{
	static inline constexpr auto size = mp11::mp_size<mp11::mp_unique<mp11::mp_list_c<T, vals...> > >::value;

	using index = T;

	static constexpr bool is_o1 = false;

	template <bool throws_on_error = false>
	static constexpr std::size_t at(index v)
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

	static constexpr bool in_range(index v) noexcept
	{
		bool found = false;
		mp11::mp_for_each<mp11::mp_unique<mp11::mp_list_c<T, vals...> > >([v, &found](auto I) {
			if (I.value == v)
			{
				found = true;
			}
		});
		return found;
	}
};

template <typename T, T... values>
struct default_indexer<std::integer_sequence<T, values...> > : public default_indexer<value_sequence<T, values...> >
{
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
	template <bool c, typename A1, typename... An>
	static constexpr auto at(A1 idx, An... rem) -> decltype(Arg::template at<c>(idx) * product_v<Args::size...> +
	                                                        at_computation_helper<Args...>::template at<c>(rem...))
	{
		return Arg::template at<c>(idx) * product_v<Args::size...> +
		       at_computation_helper<Args...>::template at<c>(rem...);
	}
};
template <typename Arg>
struct at_computation_helper<Arg>
{
	template <bool c, typename A1>
	static constexpr auto at(A1 idx) -> decltype(static_cast<std::size_t>(Arg::template at<c>(idx)))
	{
		return static_cast<std::size_t>(Arg::template at<c>(idx));
	}
};

template <typename T, typename = void>
struct add_default_indexer_if_needed
{
	using type = default_indexer<T>;
};
template <typename T>
struct add_default_indexer_if_needed<T, std::enable_if_t<is_indexer_v<T>, void> >
{
	using type = T;
};

template <typename T>
using add_default_indexer_if_needed_t = typename add_default_indexer_if_needed<T>::type;

template <typename... Args>
struct to_single_indexer
{
	using type = default_indexer<boost::mp11::mp_list<add_default_indexer_if_needed_t<Args>...> >;
};

template <typename Arg>
struct to_single_indexer<Arg>
{
	using type = add_default_indexer_if_needed_t<Arg>;
};

template <typename... Args>
using to_single_indexer_t = typename to_single_indexer<Args...>::type;

template <typename Arg, typename... Args>
struct default_indexer<
    boost::mp11::mp_list<Arg, Args...>,
    typename std::enable_if_t<boost::mp11::mp_all<is_indexer<Arg>, is_indexer<Args>...>::value, void> >
{
	static inline constexpr std::size_t const size = product_v<Arg::size, Args::size...>;

	// trailing return type syntax needed for SFINAE friendliness, otherwise is_indexer_invocable_with won't work
	// correctly
	template <bool throws_on_error = false, typename A1, typename... An>
	static constexpr auto at(A1 arg, An... args) noexcept(!throws_on_error)
	    -> decltype(at_computation_helper<Arg, Args...>::template at<throws_on_error>(arg, args...))
	{
		return at_computation_helper<Arg, Args...>::template at<throws_on_error>(arg, args...);
	}

	using slice_indexer = to_single_indexer_t<Args...>;
	using root_indexer = Arg;

	static constexpr bool is_o1 = root_indexer::is_o1 && slice_indexer::is_o1;

	template <typename A1, typename... An>
	static constexpr bool in_range(A1 arg, An... args) noexcept
	{
		return root_indexer::in_range(arg) && slice_indexer::in_range(args...);
	}
};

template <typename Indexer, typename L = void, typename b = std::true_type>
struct is_indexer_invocable_with_detail : public std::false_type
{
};

template <typename Indexer, typename... Args>
struct is_indexer_invocable_with_detail<
    Indexer,
    boost::mp11::mp_list<Args...>,
    std::integral_constant<
        bool,
        std::is_same_v<decltype(Indexer::template at<false>(std::declval<Args>()...)), std::size_t> > > :
    public std::true_type
{
};

template <typename Indexer, typename... Args>
struct is_indexer_invocable_with : public is_indexer_invocable_with_detail<Indexer, boost::mp11::mp_list<Args...> >
{
};

template <typename Indexer, typename... Args>
constexpr bool is_indexer_invocable_with_v = is_indexer_invocable_with<Indexer, Args...>::value;

template <typename Arg>
struct wrong_indexer
{
	static_assert(std::is_same_v<Arg, std::true_type>, "There is a problem with the indexer");
};

template <typename Indexer, typename Arg, typename b = std::true_type>
struct has_root_indexer : public std::false_type
{
};

template <typename Indexer, typename Arg>
struct has_root_indexer<Indexer, Arg, std::integral_constant<bool, is_indexer_invocable_with_v<typename Indexer::root_indexer, Arg> > > :
    public std::true_type
{
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

// clang-format off
namespace jbc::indexed_array::concepts
{
template <typename Indexer, typename... Args>
concept indexer_invocable_with = requires (Indexer i, Args... args)
{
	{ i.template at<true>(args...) } -> ::std::same_as<::std::size_t>;
};
} // jbc::indexed_array::concepts
// clang-format on

#endif // JBC_INDEXED_ARRAY_DETAIL_DEFAULT_INDEXER_H
