#·Copyright·2022·Julien Blanc
#·Distributed·under·the·Boost·Software·License,·Version·1.0.
#·https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

message("Boost libraries found : ${Boost_LIBRARIES}")

function(add_single_test target )
	add_executable(${target} ${ARGN})
	target_include_directories(${target} PRIVATE "../include" ${Boost_INCLUDE_DIRS} ${MAGICENUM}/include ${WISEENUM} )
if (${ENABLE_COVERAGE})
	target_compile_options(${target} PRIVATE "-fprofile-arcs" "-ftest-coverage" -Wall -Wextra)
	if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
		target_compile_options(${target} PRIVATE "-Wconversion")
	endif()
	target_link_options(${target} PRIVATE "-fprofile-arcs" "-ftest-coverage" )
	set(Gcov_LIBRARIES gcov)
endif()
	target_link_libraries(${target} ${Boost_LIBRARIES} ${Gcov_LIBRARIES} )
	target_compile_definitions(${target} PRIVATE ${definitions})
	add_test(NAME test_${target} COMMAND ${target} )
endfunction()

set(definitions -DINDEXED_ARRAY_HAS_DESCRIBE)
add_single_test(interval_ints interval_ints.cpp)
add_single_test(interval_enums interval_enums.cpp)
add_single_test(safe_initialization safe_initialization.cpp)
add_single_test(sequence_index sequence_index.cpp)
add_single_test(array_initialization array_initialization.cpp)
add_single_test(array_iteration array_iteration.cpp)
add_single_test(triviality triviality.cpp)
add_single_test(multidimensional multidimensional.cpp)
add_single_test(indexer indexer.cpp)
add_single_test(indexed_span indexed_span.cpp)
add_single_test(array_operations array_operations.cpp)
add_single_test(copy_operations copy_operations.cpp)
add_single_test(non_default_constructible non_default_constructible.cpp)
add_single_test(foreach foreach.cpp)
add_single_test(bitset indexed_bitset.cpp)
add_single_test(safe_numerics safe_numerics.cpp)
add_single_test(chrono chrono.cpp)
set_property(TARGET chrono PROPERTY CXX_STANDARD 20)
add_single_test(lambda_indexer lambda_indexer.cpp)
set_property(TARGET lambda_indexer PROPERTY CXX_STANDARD 20)

set(definitions -DINDEXED_ARRAY_HAS_MAGIC_ENUM)
add_single_test(magic_enum magic_enum.cpp)

set(definitions -DINDEXED_ARRAY_HAS_WISE_ENUM)
add_single_test(wise_enum wise_enum.cpp)
