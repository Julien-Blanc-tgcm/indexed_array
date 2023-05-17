.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

Multidimensional indexing
=========================

Rationale
---------

Multidimensional arrays can always be implemented in terms of arrays of
arrays. This is true for ``indexed_array`` as well:

.. code-block:: cpp
    :caption: An indexed array of indexed_array

    indexed_array<indexed_array<int, index_range<1, 5>>, index_range<1, 5>> data;
    static_assert(sizeof(data) == 25 * sizeof(int));

This, however, has some caveats. It makes it impossible to iterate through the
whole raw array of data. It is also less convenient to use. It also does not make very
visible what ultimately the array contains. C++23 has allowed subscript in
``operator[]`` for these very reasons, and an ``mdspan`` (multi-dimensional span) as
well (which aims at solving performance critical cases, which is not the scope of
``indexed_array``). ``md_array`` is proposed for C++26.

Usage
-----

A multi-dimensional array can be seen as an array indexed by a `tuple` of values.
Its declaration should be pretty straightforward. As a convenience, the library
implements composing multiple indexers (as a variadic list of indexers) to create
a single multidimensional indexer:

Declaring a multidimensional indexed array
is very similar to declaring a single dimension:

.. code-block:: cpp
    :caption: Multidimensional indexed array

    indexed_array<int, index_range<1, 4>, Foo> data;
    assert(data.size() == 4 * 5); // 5 is the number of enum values in Foo

Accessing the elements can de bone by two ways:

.. code-block:: cpp
    :caption: Example of multidimensional item access

    int v1 = data.at(2, Foo::bar1); // bound-checked access
    int v2 = data[{2, Foo::bar1}]; // unchecked access
    // this one requires C++23
    int v3 = data[2, Foo::bar1]; // unchecked access

The data layout is the following:

+--------------+--------------+-----+--------------+
| 1, Foo::bar1 | 1, Foo::bar2 | ... | 1, Foo::bar4 |
+--------------+--------------+-----+--------------+
| 2, Foo::bar1 | 2, Foo::bar2 | ... | 2, Foo::bar4 |
+--------------+--------------+-----+--------------+
| 3, Foo::bar1 | 3, Foo::bar2 | ... | 3, Foo::bar4 |
+--------------+--------------+-----+--------------+
| 4, Foo::bar1 | 4, Foo::bar2 | ... | 4, Foo::bar4 |
+--------------+--------------+-----+--------------+

It is also possible to get a view on a single row:

.. code-block:: cpp
    :caption: Get a view on a row

    auto second_row = data[2];
    int v3 = second_row[Foo::bar3];

There is no limit on the number of dimensions.

.. code-block:: cpp
    :caption: Multiple dimensions
    
    indexed_array<int, index_range<1, 5>, index_range<1, 5> > data;
    static_assert(sizeof(data) == 25 * sizeof(int));

Data can then be accessed this way:

.. code-block:: cpp
    
    auto ret = data[{2,3}];
    ret = data.at(2, 3);
    // C++23 only
    ret = data[2, 3];
    static_assert(is_same_v<decltype(ret), int>);

This works with enums as well:

.. code-block:: cpp
    
    indexed_array<int, Floor, index_range<1, 5> > data;
    
    auto ret = data[{Floor::F4, 3}];
    static_assert(is_same_v<decltype(ret), int>);

The number of dimensions is not limited.

Size and iteration
------------------

The ``size()`` function returns the **total amount** of items in the multidimensional
array. This means that the following holds true:

.. code-block:: cpp
    
    size() == sizeof(*this) / sizeof(value_type);
    indexed_array<int, index_range<1, 5>, index_range<1, 5> > data;
    assert(data.size() == 25);

Iteration is done through the whole raw underlying array, without any hierarchy.

.. code-block:: cpp
    
    indexed_array<int, index_range<1, 5>, index_range<1, 5> > data;
    static_assert(is_same_v<decltype(*d.begin()), int>);
    int i = 0;
    for(auto &d : data)
    {
    	d = i++;
    }
    assert(i == 25);

The layout is row major:

.. code-block:: cpp
    
    assert(data[{1, 1}] == 0);
    assert(data[{1, 2}] == 1);
    assert(data[{2, 1}] == 5);

Iterating through single “rows”
-------------------------------

The whole point of having multiple dimensions is to allow to retrieve a single
“row” at some point (otherwise, why bother with multiple dimensions in the first
place). In ``indexed_array``, this is done through the ``slice`` (or ``slice_at``) call:

.. code-block:: cpp
    
    indexed_array<int, index_range<1, 3>, index_range<1, 5> > data;
    int i = 0;
    for(auto &d : data)
    	d = i++;
    auto v = data.slice(2);
    assert(v[1] == 5);
    assert(v[5] == 9);

This call returns an ``indexed_span<int, index_range<1, 5>>``. This is a view
(a non-owning structure) on the data held in the ``indexed_array``, which
follows the same accessing scheme.

``indexed_span`` provides an interface similar to ``std::span`` (with static
extent), with the addition of the ``slice``/``slice_at`` methods if its extent
is higher than one. But it follows the ``indexed_array`` indexing scheme: it
can be used with enum values, or with non-zero starting integers, etc.

Custom multidimensional indexer
-------------------------------

When writing a custom multidimensional indexer, we must provide two additional members:

* ``root_indexer``, which is the indexer for the top-level dimensions
* ``slice_indexer``, which is the indexer for the remaining dimensions

.. code-block:: cpp
    
    // 3-dimensional indexer with top-level reverse fibonnaci indexing
    struct fib_multidim_indexer
    {
    	using index = mp_list<int, int, int>;
    	using root_indexer = reverse_fibonnaci_index;
    	using slice_indexer = make_default_indexer<index_range<3, 12>, index_range<-3, 2>>;
    	inline static constexpr size_t const size = root_indexer::size * slice_indexer::size;
    	template<bool b>
    	static constexpr auto at(int v1, int v2, int v3) noexcept(!b)
    	{
    		return root_indexer::at<b>(v1) * slice_indexer::size + slice_indexer::at<b>(v2, v3);
    	};
    };

    indexed_array<int, fib_multidim_indexer> data;

It is usually not necessary to write multidimensional indexers, since the default indexer
handles composing indexers to create a multidimensional one. This, however, can be done,
for example for performance reasons.
