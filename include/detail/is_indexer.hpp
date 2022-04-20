//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_IS_INDEXER_H
#define JBC_INDEXED_ARRAY_DETAIL_IS_INDEXER_H

#include <type_traits>

namespace jbc::indexed_array::detail
{

template <typename T, typename T2 = void>
struct is_indexer : public std::false_type
{
};

template <typename T>
struct is_indexer<T, std::enable_if_t<std::is_integral<decltype(T::size)>::value, void> > : public std::true_type
{
};

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_IS_INDEXER_H
