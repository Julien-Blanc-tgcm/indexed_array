//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#include "include/indexed_array.hpp"

#include <boost/describe.hpp>
#include <iostream>
#include <memory>
#include <utility>

using namespace std;
using namespace boost;
using namespace jbc::indexed_array;

enum class Toto
{
	First = 2,
	Second = 3,
	Last = 5
};

BOOST_DESCRIBE_ENUM(Toto, First, Second, Last);

enum class Toto2
{
	First = -1,
	Second = 0,
	Third = 1,
	Next = 2,
	Last = 2
};

BOOST_DESCRIBE_ENUM(Toto2, First, Second, Third, Next, Last)

struct custom_index
{
	using index = int;
	inline static constexpr size_t const size = 4;
	template <bool>
	static constexpr auto at(index v)
	{
		if (v <= 0)
			return 0;
		if (v < 10)
			return 1;
		if (v < 20)
			return 2;
		return 3;
	}
};

template <typename T1>
int sum(T1&& v)
{
	return v;
}

template <typename T, typename... Args>
int sum(T&& v, Args&&... remaining)
{
	return v + sum(std::forward<Args>(remaining)...);
}

template <typename T1>
int sum2(std::unique_ptr<T1> const& v)
{
	return *v;
}

template <typename T, typename... Args>
int sum2(std::unique_ptr<T> const& v, Args&&... remaining)
{
	return *v + sum2(std::forward<Args>(remaining)...);
}

template <typename... T>
int f(T&&... args)
{
	using indexer = detail::default_indexer<Toto>;
	static_assert(detail::correct_index<indexer, T::index...>(), "Args index must be consistent");
	return sum(static_cast<typename T::value_type>(args)...);
}

template <typename T1, typename... T>
int f2(T1&& arg, T&&... args)
{
	using indexer = detail::default_indexer<std::decay_t<decltype(T1::index)> >;
	static_assert(detail::correct_index<indexer, T1::index, T::index...>(), "Args index must be consistent");
	return sum2(static_cast<typename T1::value_type>(arg), static_cast<typename T::value_type>(args)...);
}

int main(int argc, char** /*argv*/)
{
	static_assert(boost::describe::has_describe_enumerators<Toto>::value);
	indexed_array<int, Toto> index{2, 3, 4};
	cout << "Hello World ! (" << index[Toto::First] << ")" << endl;
	cout << "Hello World ! (" << index[static_cast<Toto>(3)] << ")" << endl;
	cout << "Hello World ! (" << index.at(static_cast<Toto>(argc)) << ")" << endl;

	static_assert(detail::is_contiguous_sequence<std::integral_constant<int, 2> >::value);

	static_assert(
	    detail::is_contiguous_sequence<std::integral_constant<int, 2>, std::integral_constant<int, 3> >::value);

	static_assert(detail::is_contiguous_sequence<std::integral_constant<int, 2>,
	                                             std::integral_constant<int, 3>,
	                                             std::integral_constant<int, 4> >::value);

	static_assert(!detail::is_contiguous_sequence<std::integral_constant<int, 2>,
	                                              std::integral_constant<int, 4>,
	                                              std::integral_constant<int, 5> >::value);

	static_assert(!detail::is_contiguous_sequence<boost::describe::describe_enumerators<Toto> >::value);

	static_assert(detail::is_contiguous_sequence<boost::describe::describe_enumerators<Toto2> >::value);

	indexed_array<int, Toto2> idx{2, 3, 4, 5};

	cout << "Toto2: " << idx[Toto2::Third] << endl;

	indexed_array<int, std::index_sequence<1, 2, 3, 4, 5, 6> > arr{2, 3, 4, 6, 7, 8};
	indexed_array<int, std::index_sequence<1, 2, 3, 4, 6, 7> > arr2{2, 3, 4, 6, 7, 8};
	indexed_array<int, std::integer_sequence<int, -2, -1, 0, 1, 2, 3> > arr3{2, 3, 4, 6, 7, 8};

	cout << arr[2] << " " << arr[3] << " " << arr[4] << " " << arr[5] << " " << arr[6] << std::endl;
	cout << arr2[2] << " " << arr2[3] << " " << arr2[4] << " " << arr2[6] << " " << arr2[7] << std::endl;
	cout << arr3[-2] << " " << arr3[-1] << " " << arr3[0] << " " << arr3[1] << " " << arr3[2] << std::endl;

	indexed_array<int, interval<3, 5> > arr4{-2, 8, -4};
	cout << arr4[3] << " " << arr4[4] << " " << arr4[5] << std::endl;

	indexed_array<int, interval<Toto2::Third, Toto2::Next> > arr5{7, 8};
	cout << arr5[Toto2::Third] << " " << arr5[Toto2::Next] << std::endl;

	indexed_array<char const*, custom_index> const cust{"toto", "tutu", "tata", "titi"};
	cout << cust[0] << " " << cust[11] << " " << cust[13] << std::endl;

	for (auto c : cust)
	{
		cout << c;
	}
	cout << std::endl;

	indexed_array<int, interval<-10, 30> > arr6{7, 8};
	for (auto c : arr6)
	{
		cout << c << " ";
	}
	cout << std::endl;

	std::cout << f(safe_arg<Toto::First>(34), safe_arg<Toto::Second>(31), safe_arg<Toto::Last>(4)) << std::endl;

	std::cout << f2(safe_arg<Toto2::First>(std::make_unique<int>(34)),
	                safe_arg<Toto2::Second>(std::make_unique<int>(22)),
	                safe_arg<Toto2::Third>(std::make_unique<int>(4)),
	                safe_arg<Toto2::Last>(std::make_unique<int>(1)))
	          << std::endl;

	indexed_array<char const*, Toto2> arr7{safe_arg<Toto2::First>("First"),
	                                       safe_arg<Toto2::Second>("Second"),
	                                       safe_arg<Toto2::Third>("Third"),
	                                       safe_arg<Toto2::Last>("Last")};
	cout << arr7[Toto2::First] << " " << arr7[Toto2::Second] << " " << arr7[Toto2::Third] << " " << arr7[Toto2::Last]
	     << std::endl;

	indexed_array<std::unique_ptr<std::string>, Toto2> arr8{safe_arg<Toto2::First>(std::make_unique<std::string>("F")),
	                                                        safe_arg<Toto2::Second>(std::make_unique<std::string>("G")),
	                                                        safe_arg<Toto2::Third>(std::make_unique<std::string>("H")),
	                                                        safe_arg<Toto2::Last>(std::make_unique<std::string>("I"))};
	cout << *arr8[Toto2::First] << " " << *arr8[Toto2::Second] << " " << *arr8[Toto2::Third] << " "
	     << *arr8[Toto2::Last] << std::endl;
	cout << *at(arr8, Toto2::First) << " " << *arr8.front() << " " << *arr8.back() << std::endl;

	indexed_array<std::unique_ptr<std::string>, Toto2> arr9;

	using crazy_list = std::integer_sequence<int, -3, -4, 4, -2, -2, 5, -4>;
	indexed_array<int, crazy_list> t;
	static_assert(t.size() == 5);

	//	std::array<int, 3> index{2, 3, 4};
	//	cout << "Hello World ! (" << index[argc] << ")" << endl;
	return 0;
}
