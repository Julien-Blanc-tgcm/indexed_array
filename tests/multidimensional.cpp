#define BOOST_TEST_MODULE Indexed Array
#include <boost/describe.hpp>
#include <boost/test/unit_test.hpp>

#include "jbc/indexed_array.hpp"

#include <string>
#include <type_traits>

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

enum class Material
{
	Wood = 0,
	Metal,
	Stone,
	Leather
};

BOOST_DESCRIBE_ENUM(Material, Wood, Metal, Stone, Leather);

BOOST_AUTO_TEST_CASE(bidimensional)
{
	indexed_array<std::string, Color, Material> arr;
	using T = decltype(arr)::indexer;
	static_assert(std::is_same_v<T::root_indexer, T::root_indexer>, "vv");
	static_assert(detail::has_root_indexer<T, Color>::value, "Has root indexer");
	static_assert(!detail::has_root_indexer<T, int>::value, "Wrong signature results in no root indexer");
	BOOST_TEST(arr.size() == 20);
}

BOOST_AUTO_TEST_CASE(bidimensional2)
{
	indexed_array<std::string, detail::default_indexer<Color>, Color> arr;
	BOOST_TEST(arr.size() == 25);
}

BOOST_AUTO_TEST_CASE(bidimensional3)
{
	indexed_array<std::string, Color, index_range<3, 8> > arr;
	BOOST_TEST(arr.size() == 30);
	int i = 0;
	for (auto& a : arr)
	{
		a = std::to_string(i++);
	}
	BOOST_TEST(arr.at(Color::Red, 3) == "0");
	BOOST_TEST((arr[std::make_tuple(Color::White, 8)] == "29"));
	BOOST_TEST(arr.at(Color::Blue, 5) == "14");
}

struct custom_indexer
{
	using index = int;
	static inline constexpr std::size_t size = 8;
	template <bool c>
	static constexpr std::size_t at(int v)
	{
		if constexpr (c)
		{
			if (!in_range(v))
				throw std::out_of_range("Invalid index");
		}
		return v - 3;
	}
	static constexpr bool is_o1 = true;
	static constexpr bool in_range(int v)
	{
		return v >= 3 && v <= 10;
	}
};

BOOST_AUTO_TEST_CASE(multidimensional)
{
	indexed_array<std::unique_ptr<std::string>, Color, index_range<3, 8>, custom_indexer, Material> arr;
	BOOST_TEST(arr.size() == 5 * 6 * 8 * 4);
	int i = 0;
	for (auto& a : arr)
	{
		a = std::make_unique<std::string>(std::to_string(i++));
	}
	BOOST_TEST(*arr.at(Color::White, 4, 6, Material::Stone) == "814");
}

BOOST_AUTO_TEST_CASE(multidimensional_init)
{
	indexed_array<std::unique_ptr<std::string>, index_range<3, 4>, index_range<5, 6>, index_range<7, 8> > arr{
	    std::make_unique<std::string>("357"),
	    std::make_unique<std::string>("358"),
	    std::make_unique<std::string>("367"),
	    std::make_unique<std::string>("368"),
	    std::make_unique<std::string>("457"),
	    std::make_unique<std::string>("458"),
	    std::make_unique<std::string>("467"),
	    std::make_unique<std::string>("468"),
	};
	BOOST_TEST(*arr.at(4, 5, 7) == "457");
}

BOOST_AUTO_TEST_CASE(multidimensional_operator_bracket)
{
	indexed_array<std::unique_ptr<std::string>, index_range<3, 4>, index_range<5, 6>, index_range<7, 8> > arr{
	    std::make_unique<std::string>("357"),
	    std::make_unique<std::string>("358"),
	    std::make_unique<std::string>("367"),
	    std::make_unique<std::string>("368"),
	    std::make_unique<std::string>("457"),
	    std::make_unique<std::string>("458"),
	    std::make_unique<std::string>("467"),
	    std::make_unique<std::string>("468"),
	};
	BOOST_TEST(*arr(4, 5, 7) == "457");
	BOOST_TEST((*arr[std::tuple{4, 5, 7}] == "457"));
}

BOOST_AUTO_TEST_CASE(multidimensional_safe_initialization)
{
	auto blue3 = std::make_unique<std::string>("Blue3");
	indexed_array<std::unique_ptr<std::string>, Color, index_range<3, 4> > arr{
	    safe_arg<Color::Red, 3>(std::make_unique<std::string>("Red3")),
	    safe_arg<Color::Red, 4>(std::make_unique<std::string>("Red4")),
	    safe_arg<Color::Green, 3>(std::make_unique<std::string>("Green3")),
	    safe_arg<Color::Green, 4>(std::make_unique<std::string>("Green4")),
	    safe_arg<Color::Blue, 3>(std::move(blue3)),
	    safe_arg<Color::Blue, 4>(std::make_unique<std::string>("Blue4")),
	    safe_arg<Color::Black, 3>(std::make_unique<std::string>("Black3")),
	    safe_arg<Color::Black, 4>(std::make_unique<std::string>("Black4")),
	    safe_arg<Color::White, 3>(std::make_unique<std::string>("White3")),
	    safe_arg<Color::White, 4>(std::make_unique<std::string>("White4")),
	};
	BOOST_TEST(*arr.at(Color::Blue, 4) == "Blue4");
	BOOST_TEST(*arr.at(Color::Blue, 3) == "Blue3");
}

BOOST_AUTO_TEST_CASE(multidimensional_slices)
{
	indexed_array<std::unique_ptr<std::string>, Color, Material, index_range<-3, 6> > arr{};
	int i = 0;
	for (auto& a : arr)
	{
		a = std::make_unique<std::string>(std::to_string(i++));
	}
	auto slice1 = arr.slice(Color::Green);
	BOOST_TEST(slice1.size() == 40);
	BOOST_TEST(*slice1.at(Material::Wood, 0) == "43");
	auto slice2 = slice1.slice(Material::Metal);
	BOOST_TEST(slice2.size() == 10);
	BOOST_TEST(*slice2[-3] == "50");
	BOOST_TEST(*slice2[6] == "59");
}

BOOST_AUTO_TEST_CASE(multidimensional_const_slices)
{
	indexed_array<std::unique_ptr<std::string>, Color, Material, index_range<-3, 6> > arr{};
	int i = 0;
	for (auto& a : arr)
	{
		a = std::make_unique<std::string>(std::to_string(i++));
	}
	auto const& ref = arr;
	i = 0;
	for (auto& a : ref)
	{
		BOOST_TEST(*a == std::to_string(i++));
	}
	auto slice1 = ref.slice(Color::Green);
	BOOST_TEST(slice1.size() == 40);
	BOOST_TEST(*slice1.at(Material::Wood, 0) == "43");
	auto slice2 = slice1.slice(Material::Metal);
	BOOST_TEST(slice2.size() == 10);
	BOOST_TEST(*slice2[-3] == "50");
	BOOST_TEST(*slice2[6] == "59");
}

BOOST_AUTO_TEST_CASE(multidimensional_o1)
{
	using T = indexed_array<int, index_range<-2, 5>, index_range<10, 15>, index_range<4, 8> >;
	BOOST_TEST(T::is_o1);
	using T2 = indexed_array<int, union_of<index_range<-2, 5>, index_range<10, 15> >, index_range<4, 8> >;
	BOOST_TEST(!T2::is_o1);
	using T3 = indexed_array<int, union_of<index_range<0, 5>, index_range<5, 10> > >;
	BOOST_TEST(T3::is_o1);
}

BOOST_AUTO_TEST_CASE(multidimensional_in_range)
{
	using T = indexed_array<std::unique_ptr<std::string>, Color, Material, index_range<-3, 6> >;
	BOOST_TEST(T::in_range(Color::Black, Material::Wood, 4));
	BOOST_TEST(!T::in_range(static_cast<Color>(-5), Material::Wood, 4));
	BOOST_TEST(!T::in_range(Color::White, Material::Wood, 8));
	BOOST_TEST(!T::in_range(Color::White, Material::Wood, -4));
	BOOST_TEST(T::in_range(Color::White, Material::Wood, 6));
	BOOST_TEST(T::in_range(Color::White, Material::Wood, -3));
}
