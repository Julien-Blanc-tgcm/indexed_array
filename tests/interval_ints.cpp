#define BOOST_TEST_MODULE Indexed Array
#include <boost/test/unit_test.hpp>

#include "jbc/indexed_array.hpp"

using namespace jbc::indexed_array;

BOOST_AUTO_TEST_CASE(index_range_array)
{
	int a = 2;
	indexed_array<int, index_range<2, 4> > arr{1, a, 3};
	BOOST_TEST(arr.size() == 3);
	BOOST_TEST(arr[2] == 1);
	BOOST_TEST(arr[3] == 2);
	BOOST_TEST(arr[4] == 3);
}

BOOST_AUTO_TEST_CASE(negative_index_range)
{
	indexed_array<int, index_range<-1, 1> > arr{1, 2, 3};
	BOOST_TEST(arr.size() == 3);
	BOOST_TEST(arr[-1] == 1);
	BOOST_TEST(arr[0] == 2);
	BOOST_TEST(arr[1] == 3);
}

BOOST_AUTO_TEST_CASE(out_of_bound_index_range)
{
	indexed_array<int, index_range<-6, -4> > arr{1, 2, 3};
	BOOST_TEST(arr.size() == 3);
	bool catched = false;
	int a = 0;
	try
	{
		a = arr.at(-8);
	}
	catch (std::out_of_range&)
	{
		catched = true;
	}
	BOOST_TEST(catched);
}
