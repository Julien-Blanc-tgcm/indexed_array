cmake_minimum_required(VERSION 3.5)

project(indexed_array LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(indexed_array "main.cpp" 
	"include/indexed_array.hpp"
	"include/detail/checked_arg.hpp"
	"include/detail/default_indexer.hpp"
	"include/detail/indexed_array.hpp"
	"include/detail/is_contiguous.hpp"
)
target_compile_options(indexed_array PRIVATE -Wall -Wextra -Werror)
target_include_directories(indexed_array PRIVATE $ENV{HOME}/development/Describe_ref/describe/include)
