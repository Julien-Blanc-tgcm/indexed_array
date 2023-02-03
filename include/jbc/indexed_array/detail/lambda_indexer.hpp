// ·Copyright·2022·Julien Blanc
// ·Distributed·under·the·Boost·Software·License,·Version·1.0.
// ·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_LAMBDA_INDEXER_H
#define JBC_INDEXED_ARRAY_DETAIL_LAMBDA_INDEXER_H

#include "default_indexer.hpp"
#include "is_contiguous.hpp"
#include "is_indexer.hpp"
#include "value_sequence.hpp"

#include <stdexcept>
#include <type_traits>

#include <boost/config.hpp> // for BOOST_UNLIKELY
#include <boost/mp11.hpp>

namespace jbc::indexed_array::detail
{

template<auto F, auto Value, typename T = void>
struct helper_compute_value
{
	static constexpr auto const value = Value;
};

template<auto F, auto Value>
struct helper_compute_value<F, Value,
    std::enable_if_t<
        std::is_integral_v<decltype(F(Value))>, void>>
{
	static constexpr auto const value = F(Value);
};

template <typename T, auto F, auto MinValue, type_identity_t<decltype(MinValue)> MaxValue>
struct lambda_indexer
{
	using integral_index_type = decltype(MinValue);
	static_assert(std::is_same_v<std::decay_t<decltype(F(std::declval<T>()))>,
	                             std::decay_t<decltype(helper_compute_value<F, MinValue>::value)>>,
	              "F(T) must return the same thing as MinValue");

  private:
	static constexpr auto minvalue = helper_compute_value<F,MinValue>::value;
	static constexpr auto maxvalue = helper_compute_value<F, MaxValue>::value;
	using internal_indexer_ = default_indexer<index_range<minvalue, maxvalue>>;

  public:
	static inline constexpr auto const size = internal_indexer_::size;
	static constexpr bool is_o1 = internal_indexer_::is_o1;
	using index = T;
	template <bool throws_on_error = false>
	static constexpr auto at(index v)
	{
		return internal_indexer_::at(F(v));
	}
	static constexpr bool in_range(index v) noexcept
	{
		return internal_indexer_::in_range(F(v));
	}
};

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_LAMBDA_INDEXER_H
