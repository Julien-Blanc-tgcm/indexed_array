// Copyright 2025 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_REFLECTION_H
#define JBC_INDEXED_ARRAY_DETAIL_REFLECTION_H

#include "detail/default_indexer.hpp"
#include "detail/foreach.hpp"

#include <boost/mp11.hpp>
#include <meta>
#include <tuple>

namespace jbc::indexed_array::detail
{

/*template <typename Enum>
requires (std::is_enum_v<Enum>)
struct enum_to_value_sequence
{
	template<typename T>
	consteval std::meta::info make_integer_seq_refl(T N) {
	  std::vector args{^T};
	  for (T k = 0; k < N; ++k) {
	    args.push_back(std::meta::reflect_result(k));
	  }
	  return substitute(^std::integer_sequence, args);
	}	
	using type = define_class(std::meta::substitute(^^value_sequence, std::meta::enumerators_of<^^Enum>));
	using values = make_from_index_sequence<typelist1>(std::meta::enumerators_of<^^Enum>);
	static constexpr auto type1 = std::tuple_cat(std::meta::enumerators_of<^^Enum> >);
}; */

/*template <typename Enum, template <class...> typename L, typename... Args>
struct describe_to_value_sequence<Enum, L<Args...> >
{
	using type = value_sequence<Enum, Args::value...>;
};
*/
template <typename Enum>
requires (std::is_enum_v<Enum>
#if defined(INDEXED_ARRAY_HAS_DESCRIBE)
		&& !boost::describe::has_describe_enumerators<Enum>::value
#endif
		)
struct default_indexer<Enum>
{
  private:
	static consteval auto start_offset()
	{
		if constexpr (size == 0)
			return static_cast<std::underlying_type_t<Enum>>(0);
		else
			return static_cast<std::underlying_type_t<Enum>>([:std::meta::enumerators_of(^^Enum)[0]:]);
	}
	static consteval auto is_contiguous()
	{
		bool first = true;
		using Type = std::underlying_type_t<Enum>;
		Type v{};
		template for(constexpr auto m : std::define_static_array(std::meta::enumerators_of(^^Enum)))
	        {
			if (first)
			{
				v = static_cast<Type>([:m:]);
				first = false;
			}
			else
			{
				if (static_cast<Type>([:m:]) != v && static_cast<Type>([:m:]) != (v + 1))
					return false;
				v = static_cast<Type>([:m:]);
			}
	        }
		return true;
	}
	static consteval auto size_helper()
	{
		std::size_t s = 0;
		bool first = true;
		Enum prev = {};
		template for(constexpr auto m : std::define_static_array(std::meta::enumerators_of(^^Enum)))
	        {
			if (first)
			{
				prev = [:m:];
				first = false;
			}
			else
			{
				if (prev == [:m:])
					continue;
				prev = [:m:];
			}
			s += 1;
	        }
		return s;
	}
  public:
	using index = Enum;
	static constexpr bool is_o1 = is_contiguous();
	static inline constexpr auto const size = size_helper();
	template <bool throws_on_error = false>
	static constexpr std::size_t at(Enum v) noexcept(!throws_on_error)
	{
		if constexpr (is_contiguous())
		{
			using Type = std::underlying_type_t<index>;
			auto idx = static_cast<Type>(v);
			if constexpr(throws_on_error)
			{
				if (idx >= start_offset() && (idx < start_offset() + static_cast<std::int64_t>(size)))
					return idx - start_offset();
				throw std::out_of_range("Invalid index");
			}
			return idx - start_offset();
		}
		else
		{
			std::size_t ret = 0;
			template for(constexpr auto m : std::define_static_array(std::meta::enumerators_of(^^Enum)))
		        {
				if ([:m:] == v)
					return ret;
				++ret;
		        }
			if constexpr (throws_on_error)
				throw std::out_of_range("Invalid index");
			return -1;
		}
	}
	static constexpr auto in_range(index v) noexcept
	{
		if constexpr (is_contiguous())
		{
			using Type = std::underlying_type_t<index>;
			return static_cast<Type>(v) >= start_offset() && static_cast<Type>(v) < start_offset() + size;
		}
		else
		{
			template for(constexpr auto m : std::define_static_array(std::meta::enumerators_of(^^Enum)))
		        {
				if ([:m:] == v)
					return true;
		        }
			return false;
		}
	}
};

} // namespace jbc::indexed_array::detail
#endif // JBC_INDEXED_ARRAY_DETAIL_REFLECTION_H
