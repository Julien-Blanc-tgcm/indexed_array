#define BOOST_TEST_MODULE Indexed Array
#include <boost/describe.hpp>
#include <boost/test/unit_test.hpp>

#include "jbc/indexed_array.hpp"
#include "jbc/indexed_array/describe.hpp"

using namespace jbc::indexed_array;

enum class Color
{
	Red = -4,
	Green = -1,
	Blue = 0,
	Black = 6,
	White = 8
};

BOOST_DESCRIBE_ENUM(Color, Red, Green, Blue, Black, White);

BOOST_AUTO_TEST_CASE(enum_indexing)
{
	int a = 2;
	indexed_array<int, Color> arr{
	    safe_arg<Color::Red>(10),   //
	    safe_arg<Color::Green>(-4), //
	    safe_arg<Color::Blue>(1),   //
	    safe_arg<Color::Black>(a),  //
	    safe_arg<Color::White>(3)   //
	};
	BOOST_TEST(arr.size() == 5);
	BOOST_TEST(arr[Color::Red] == 10);
	BOOST_TEST(arr[Color::Green] == -4);
	BOOST_TEST(arr[Color::Blue] == 1);
	BOOST_TEST(arr.at(Color::Black) == 2); // also verify bound check access
	BOOST_TEST(arr[Color::White] == 3);
	bool catched = false;
	try
	{
		arr.at(static_cast<Color>(888)) = 1;
	}
	catch (std::out_of_range&)
	{
		catched = true;
	}
	BOOST_TEST(catched);
}

BOOST_AUTO_TEST_CASE(integerlist_indexing)
{
	indexed_array<std::string, std::integer_sequence<int, -4, 7, -8, 6, 123> > arr{
	    safe_arg<-4>("-4"), //
	    safe_arg<7>("7"),
	    safe_arg<-8>("-8"),
	    safe_arg<6>("6"),
	    safe_arg<123>("123") //
	};
	BOOST_TEST(arr.size() == 5);
	BOOST_TEST(arr[-4] == "-4");
	BOOST_TEST(arr[7] == "7");
	BOOST_TEST(arr.at(-8) == "-8");
	BOOST_TEST(arr[6] == "6");
	BOOST_TEST(arr[123] == "123");
	bool catched = false;
	try
	{
		arr[7] = arr.at(2);
	}
	catch (std::out_of_range&)
	{
		catched = true;
	}
	BOOST_TEST(catched);
	BOOST_TEST(arr[7] == "7");
}

BOOST_AUTO_TEST_CASE(integerlist_contiguous_indexing)
{
	indexed_array<std::string, std::integer_sequence<int, 0, 1, 2, 3, 4> > arr{
	    safe_arg<0>("0"), //
	    safe_arg<1>("10"),
	    safe_arg<2>("20"),
	    safe_arg<3>("30"),
	    safe_arg<4>("40") //
	};
	BOOST_TEST(arr.size() == 5);
	BOOST_TEST(arr[0] == "0");
	BOOST_TEST(arr.at(1) == "10");
	BOOST_TEST(arr[2] == "20");
	BOOST_TEST(arr[3] == "30");
	BOOST_TEST(arr[4] == "40");
	arr.at(4) = "toto";
	BOOST_TEST(arr[4] == "toto");
	BOOST_TEST((detail::is_contiguous_sequence<boost::mp11::mp_list_c<int, 0, 1, 2, 3, 4> >::value));
}

BOOST_AUTO_TEST_CASE(integerlist_empty_sequence)
{
	indexed_array<std::string, std::integer_sequence<int> > arr{};
	BOOST_TEST(arr.size() == 0);
}

BOOST_DEFINE_ENUM_CLASS(ContEnum, A1, A2, A3, A4)

BOOST_AUTO_TEST_CASE(contiguous_enum)
{
	BOOST_TEST((detail::is_contiguous_sequence<
	            detail::describe_to_value_sequence<ContEnum,
	                                               boost::describe::describe_enumerators<ContEnum> >::type>::value));
	using T1 = indexed_array<int, ContEnum>;
	BOOST_TEST(T1::is_o1);
	using T2 = indexed_array<int, Color>;
	BOOST_TEST(!T2::is_o1);
}
