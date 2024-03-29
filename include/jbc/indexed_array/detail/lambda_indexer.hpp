// ·Copyright·2022·Julien Blanc
// ·Distributed·under·the·Boost·Software·License,·Version·1.0.
// ·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_LAMBDA_INDEXER_H
#define JBC_INDEXED_ARRAY_DETAIL_LAMBDA_INDEXER_H

#include "default_indexer.hpp"

#include <stdexcept>
#include <type_traits>

#include <boost/config.hpp> // for BOOST_UNLIKELY
#include <boost/mp11.hpp>

namespace jbc::indexed_array::detail
{

template <auto F, std::size_t number_of_items, bool is_o1_ = true>
struct lambda_indexer
{
  private:
	using internal_indexer_ = default_indexer<index_range<std::size_t(0), number_of_items - 1u> >;

  public:
	static constexpr auto const size = internal_indexer_::size;
	static constexpr bool is_o1 = is_o1_;

	// Trailing return type used is here is needed for SFINAE friendliness. Otherwise, attempts to detect if the call
	// is working (such as invocable_v) will incorrectly work. This is needed to make the trait
	// is_indexer_invocable_with work correctly
	template <bool throws_on_error, typename... Args>
	static constexpr auto at(Args... args) -> decltype(internal_indexer_::template at<throws_on_error>(F(args...)))
	{
		return internal_indexer_::template at<throws_on_error>(F(args...));
	}

	template <typename... Args>
	static constexpr bool in_range(Args... args)
	{
		return internal_indexer_::in_range(F(args...));
	}
};

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_LAMBDA_INDEXER_H
