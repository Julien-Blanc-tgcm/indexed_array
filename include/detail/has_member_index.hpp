//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#ifndef JBC_INDEXED_ARRAY_DETAIL_HAS_MEMBER_INDEX_H
#define JBC_INDEXED_ARRAY_DETAIL_HAS_MEMBER_INDEX_H

#include <type_traits>

namespace jbc::indexed_array::detail
{
template <typename T, typename T2 = void>
struct has_member_index : public std::false_type
{
};

template <typename T>
struct has_member_index<T, std::enable_if_t<std::is_class<typename T::checked_arg_index>::value, void> > :
    public std::true_type
{
};

} // namespace jbc::indexed_array::detail

#endif // JBC_INDEXED_ARRAY_DETAIL_HAS_MEMBER_INDEX_H
