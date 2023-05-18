#·Copyright·2022·Julien Blanc
#·Distributed·under·the·Boost·Software·License,·Version·1.0.
#·https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5)

if (NOT "${CMAKE_CXX_STANDARD}")
	set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOSTROOT "$ENV{HOME}/development/boost_1.78" CACHE STRING "Where to find boost")
set(MAGICENUM "${CMAKE_CURRENT_SOURCE_DIR}/magic_enum" CACHE STRING "Where to find magic enum")
set(WISEENUM "${CMAKE_CURRENT_SOURCE_DIR}/wise_enum" CACHE STRING "Where to find wise enum")
set(ENABLE_COVERAGE OFF CACHE BOOL "Enable coverage generation")

project(indexed_array LANGUAGES CXX)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	add_compile_options(-stdlib=libc++)
	add_link_options(-stdlib=libc++)
endif()

find_package(Boost 1.78.0 REQUIRED)

enable_testing()

add_executable(indexed_array "main.cpp" 
	"include/jbc/indexed_array.hpp"
	"include/jbc/indexed_array/describe.hpp"
	"include/jbc/indexed_array/magic_enum.hpp"
	"include/jbc/indexed_array/wise_enum.hpp"
	"include/jbc/indexed_array/detail/checked_arg.hpp"
	"include/jbc/indexed_array/detail/default_indexer.hpp"
	"include/jbc/indexed_array/detail/index_range.hpp"
	"include/jbc/indexed_array/detail/indexed_array.hpp"
	"include/jbc/indexed_array/detail/indexed_bitset.hpp"
	"include/jbc/indexed_array/detail/indexed_span.hpp"
	"include/jbc/indexed_array/detail/is_contiguous.hpp"
	"include/jbc/indexed_array/detail/is_indexer.hpp"
	"include/jbc/indexed_array/detail/lambda_indexer.hpp"
	"include/jbc/indexed_array/detail/union_of.hpp"
	"include/jbc/indexed_array/detail/value_sequence.hpp"
)
target_compile_options(indexed_array PRIVATE -Wall)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	target_compile_options(indexed_array PRIVATE "-Wconversion" "-Wextra")
endif()
target_include_directories(indexed_array PRIVATE ${Boost_INCLUDE_DIRS} ${MAGICENUM}/include ${WISE_ENUM} )
target_compile_definitions(indexed_array PRIVATE -DINDEXED_ARRAY_HAS_DESCRIBE)

add_subdirectory(tests)

# documentation generation
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
find_package(Doxygen)
find_package(Sphinx)

if(${Doxygen_FOUND})
	set(DOXYGEN_GENERATE_XML YES)
	set(DOXYGEN_GENERATE_HTML YES)
	doxygen_add_docs(doc ${SOURCES} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)

	if (${Sphinx_FOUND})
		set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/doc)
		set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/sphinx)

		file(GLOB doc_sources "${CMAKE_CURRENT_SOURCE_DIR}/doc/*.rst")

		add_custom_target(Sphinx
			COMMAND
			${SPHINX_EXECUTABLE} -b html -c ${CMAKE_CURRENT_SOURCE_DIR}/sphinx_conf
			-Dbreathe_projects.indexed_array=${CMAKE_CURRENT_BINARY_DIR}/xml
			${SPHINX_SOURCE} ${SPHINX_BUILD}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating documentation with Sphinx"
			DEPENDS doc
			SOURCES ${doc_sources}
		)
	endif()
endif()

