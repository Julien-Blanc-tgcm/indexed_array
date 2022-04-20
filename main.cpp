//·Copyright·2022·Julien Blanc
//·Distributed·under·the·Boost·Software·License,·Version·1.0.
//·https://www.boost.org/LICENSE_1_0.txt

#include "include/indexed_array.hpp"

#include <boost/describe.hpp>

#include <iostream>
#include <memory>
#include <tuple>
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
	template <bool c = true>
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

struct custom_index2
{
	using index = mp11::mp_list<int, int>;
	inline static constexpr size_t const size = 4;
	template <bool c = true>
	static constexpr auto at(int v1, int v2)
	{
		auto res = v1 * 2 + v2;
		if constexpr (c)
		{
			if (res < 0 || static_cast<std::size_t>(res) >= size)
				throw std::out_of_range("Invalid index");
		}
		return res;
	}
	using slice_indexer = detail::add_default_indexer_t<interval<0, 1>>;
	using root_indexer = detail::add_default_indexer_t<interval<0, 1>>;
};

struct custom_index3
{
	using index = mp11::mp_list<int, Toto2>;
	inline static constexpr size_t const size = 8;
	template <bool c = true>
	static constexpr auto at(int v1, Toto2 v2)
	{
		auto res = v1 * 4 + static_cast<int>(v2) + 1;
		if constexpr (c)
		{
			if (res < 0 || static_cast<std::size_t>(res) >= size)
				throw std::out_of_range("Invalid index");
		}
		return res;
	}
	using slice_indexer = detail::add_default_indexer_t<Toto2>;
	using root_indexer = detail::add_default_indexer_t<interval<-1, 0>>;
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
	static_assert(detail::correct_index<indexer, typename T::checked_arg_index...>(), "Args index must be consistent");
	return sum(static_cast<typename T::value_type>(args)...);
}

template <typename T1, typename... T>
int f2(T1&& arg, T&&... args)
{
	using indexer = detail::default_indexer<std::decay_t<decltype(T1::checked_arg_index::value)> >;
	static_assert(detail::correct_index<indexer, typename T1::checked_arg_index, typename T::checked_arg_index...>(),
	              "Args index must be consistent");
	return sum2(static_cast<typename T1::value_type>(arg), static_cast<typename T::value_type>(args)...);
}

enum class Color
{
	Red = 0,
	Green = 1,
	Blue = 2,
	White = 10,
	Gray = 11,
	Black = 12
};

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

	static_assert(
	    std::is_same<detail::union_of<interval<2, 3>, interval<7, 8> >, integer_sequence<int, 2, 3, 7, 8> >::value,
	    "Same type");

	std::cout << typeid(detail::union_of<interval<2, 3>, interval<7, 8> >).name() << std::endl;

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

	indexed_array<int, union_of<interval<Color::Red, Color::Green>, interval<Color::White, Color::Black> > > unarr;
	static_assert(unarr.size() == 5);
	unarr.at(Color::Red) = 1;
	unarr.at(Color::Green) = 2;
	unarr.at(Color::White) = 3;
	unarr.at(Color::Gray) = 4;
	unarr.at(Color::Black) = 5;
	for (auto a : unarr)
	{
		std::cout << a;
	}
	std::cout << std::endl;
	indexed_array<int, union_of<interval<Color::Red, Color::Blue>, single_value<Color::White> > > unarr2{
	    safe_arg<Color::Red>(1), safe_arg<Color::Green>(2), safe_arg<Color::Blue>(3), safe_arg<Color::White>(4)};
	static_assert(unarr2.size() == 4);
	for (auto a : unarr2)
	{
		std::cout << a;
	}
	std::cout << std::endl;
	static_assert(std::is_trivially_copyable<decltype(unarr2)>::value, "Triviality preserved");
	static_assert(std::is_trivially_destructible<decltype(unarr2)>::value, "Triviality preserved");
	static_assert(std::is_trivially_constructible<decltype(unarr2)>::value, "Triviality preserved");
	static_assert(std::is_trivially_move_constructible<decltype(unarr2)>::value, "Triviality preserved");
	static_assert(std::is_trivially_move_assignable<decltype(unarr2)>::value, "Triviality preserved");
	static_assert(std::is_trivially_default_constructible<decltype(unarr2)>::value, "Triviality preserved");
	static_assert(std::is_trivially_copy_assignable<decltype(unarr2)>::value, "Triviality preserved");
	static_assert(std::is_trivially_copy_constructible<decltype(unarr2)>::value, "Triviality preserved");
	// static_assert(std::is_trivial_v<std::decay_t<decltype(unarr2)> >, "Triviality preserved");

	static_assert(!std::is_trivial<decltype(arr8)>::value, "Triviality preserved (string array not trivial)");

	static_assert(std::is_trivial_v<std::array<int, 12> >, "array is trivial");

	indexed_array<int, custom_index2> multidim{
	    safe_arg<0, 0>(10), safe_arg<0, 1>(20), safe_arg<1, 0>(30), safe_arg<1, 1>(40)};
	std::cout << multidim.at(0, 0) << multidim.at(0, 1) << multidim[{1, 0}] << multidim[{1, 1}] << std::endl;
	try
	{
		std::cout << multidim.at(2, 0) << multidim.at(0, 1) << multidim[{1, 0}] << multidim[{1, 1}] << std::endl;
	}
	catch (std::out_of_range&)
	{
		std::cout << "catched" << multidim[{0, 1}] << multidim[{1, 0}] << multidim[{1, 1}] << std::endl;
	}
	//	std::array<int, 3> index{2, 3, 4};
	//	cout << "Hello World ! (" << index[argc] << ")" << endl;
	indexed_array<int, custom_index3> multidim2{0, 1, 2, 3, 4, 5, 6, 7};
	std::cout << multidim2.at(0, Toto2::First) << multidim2.at(1, Toto2::Second) << std::endl;

	indexed_array<int, interval<0, 2>, interval<0, 2>, interval<3, 4> > multidim3;
	static_assert(multidim3.size() == 18);
	int i = 0;
	for (auto& c : multidim3)
		c = i++;

	for (i = 0; i <= 2; ++i)
	{
		for (int j = 0; j <= 2; ++j)
		{
			std::cout << "(";
			for (int k = 3; k <= 4; ++k)
			{
				std::cout << multidim3.at(i, j, k) << " ";
			}
			std::cout << ")";
		}
		std::cout << std::endl;
	}
	std::cout << multidim3.at(0, 0, 3) << " " << multidim3.at(1, 1, 4) << std::endl;

	indexed_array<int, Toto2, interval<4, 8>, union_of<interval<1, 2>, single_value<4> > > ttt;
	static_assert(ttt.size() == 4 * 5 * 3, "Correct size");
	i = 0;
	for (auto& c : ttt)
	{
		c = i++;
	}
	std::cout << ttt.at(Toto2::Last, 6, 4) << " " << ttt.at(Toto2::First, 4, 1) << std::endl;
	std::cout << (static_cast<int>(Toto2::Last) + 1) * 5 * 3 + (6 - 4) * 3 + 2 << std::endl;

	indexed_array<int, Toto2> ttt3;
	static_assert(ttt3.size() == 4);

	indexed_array<int, std::integer_sequence<Toto2, Toto2::First, Toto2::Second> > ttt5{3, 4};
	static_assert(ttt5.size() == 2);
	std::cout << ttt5[Toto2::First] << " " << ttt5.at(Toto2::Second) << std::endl;

	static_assert(detail::is_contiguous_sequence<std::integral_constant<Toto2, Toto2::First>,
	                                             std::integral_constant<Toto2, Toto2::Second>,
	                                             std::integral_constant<Toto2, Toto2::Third> >::value,
	              "contiguous");
	return 0;
}
