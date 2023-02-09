// Copyright 2022 Julien Blanc
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_IS_INDEXER_H
#define JBC_INDEXED_ARRAY_DETAIL_IS_INDEXER_H

#include <type_traits>
#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
#include <concepts>
#endif

namespace jbc::indexed_array::detail
{

template <typename T, typename T2 = void>
struct is_indexer : public std::false_type
{
};

template <typename T>
struct is_indexer<
    T,
    std::enable_if_t<std::is_integral<decltype(T::size)>::value && std::is_integral<decltype(T::is_o1)>::value,
                     void> > : public std::true_type
{
};

template <typename T>
constexpr bool is_indexer_v = is_indexer<T>::value;

} // namespace jbc::indexed_array::detail

#if defined(__cpp_concepts) && __cpp_concepts >= 202002L
namespace jbc::indexed_array::concepts
{
template <typename T>
concept indexer = ::jbc::indexed_array::detail::is_indexer_v<T>;
} // namespace jbc::indexed_array::concepts
#endif


#endif // JBC_INDEXED_ARRAY_DETAIL_IS_INDEXER_H
