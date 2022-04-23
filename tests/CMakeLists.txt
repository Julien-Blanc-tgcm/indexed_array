#·Copyright·2022·Julien Blanc
#·Distributed·under·the·Boost·Software·License,·Version·1.0.
#·https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

function(add_single_test target)
	add_executable(${target} ${ARGN})
	target_link_libraries(${target} ${Boost_LIBRARIES} gcov)
	target_include_directories(${target} PRIVATE "../include" ${Boost_INCLUDE_DIRS} )
	target_compile_options(${target} PRIVATE "-fprofile-arcs" "-ftest-coverage" )
	target_link_options(${target} PRIVATE "-fprofile-arcs" "-ftest-coverage" )
	add_test(NAME test_${target} COMMAND ${target} )
endfunction()

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
