#·Copyright·2022·Julien Blanc
#·Distributed·under·the·Boost·Software·License,·Version·1.0.
#·https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5)

set(BOOSTROOT "$ENV{HOME}/development/boost_1.78" CACHE STRING "Where to find boost")
set(MAGICENUM "${CMAKE_CURRENT_SOURCE_DIR}/magic_enum" CACHE STRING "Where to find magic enum")
set(WISEENUM "${CMAKE_CURRENT_SOURCE_DIR}/wise_enum" CACHE STRING "Where to find wise enum")

project(indexed_array LANGUAGES CXX)

find_package(Boost 1.78.0 REQUIRED)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(indexed_array "main.cpp" 
	"include/indexed_array.hpp"
	"include/detail/checked_arg.hpp"
	"include/detail/default_indexer.hpp"
	"include/detail/has_member_index.hpp"
	"include/detail/indexed_array.hpp"
	"include/detail/indexed_span.hpp"
	"include/detail/is_contiguous.hpp"
	"include/detail/is_indexer.hpp"
	"include/detail/magic_enum.hpp"
	"include/detail/union_of.hpp"
)
target_compile_options(indexed_array PRIVATE -Wall -Wextra -Werror)
target_include_directories(indexed_array PRIVATE ${Boost_INCLUDE_DIRS} ${MAGICENUM}/include ${WISE_ENUM} )
target_compile_definitions(indexed_array PRIVATE -DINDEXED_ARRAY_HAS_DESCRIBE)

add_subdirectory(tests)
