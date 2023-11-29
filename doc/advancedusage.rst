.. Copyright 2022 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

.. _advancedusage:

Advanced usage
==============

Non-contiguous indexes
----------------------

``indexed_array`` supports a special form of indexes, which are non-contiguous. An example of
a non-contiguous index is the sequence ``{1, 2, 4, 8}``. This sequence can be mapped to
``{0, 1, 2, 3}`` which can then be used as the internal index for the internal array.

Using non-contiguous indexes with integer values is highly non-recommended. But using them
with enums addresses a few uncommon use cases in a simple and elegant way.

An important note to consider is that when using non-contiguous indexes, access to the
elements in the ``indexed_array`` is no longer ``O(1)`` but becomes ``O(N)``. This is because the
indexer must iterate through all values, incrementing the inner index, to retrieve the
corresponding value.

``indexed_array`` provides the static member ``is_o1`` which is true if the ``indexed_array`` has ``O(1)``
access, and false otherwise. This allows putting a ``static_assert`` in case ``O(1)`` access is a
requirement, or to enable some optimizations based on this property.

Integer sequences
^^^^^^^^^^^^^^^^^

The library provides the template class ``value_sequence``, which is very similar to
``std::integer_sequence``, but does not require the type parameter to be an integer-like
type. The library uses this class for indexing using a sequence of values, and provides
automatic conversion from ``std::integer_sequence``, so that we can write:

.. code-block:: cpp
    :caption: Declaration of a array using an integer sequence indexing

    using T = indexed_array<int, std::integer_sequence<int, -1, 0, 1, 2, 3>>;
    assert(T::size() == 5);

This works as well if the values are not contiguous:

.. code-block:: cpp
    :caption: Non-contiguous sequence of values

    using T = indexed_array<int, std::integer_sequence<int, 1, 2, 4, 8>>;
    assert(T::size() == 4);

Non contiguous enum values
^^^^^^^^^^^^^^^^^^^^^^^^^^

Sometimes enum values are not contiguous, because some value is reserved or no longer in use.

.. code-block:: cpp
    :caption: Non-contiguous enum values

    enum class MotorType
    {
    	LocalManufacturer = 1,
    	// we no longer work with BadManufacturer, we don't handle the value in this product
    	// but it is reserved to avoid confusion, other products still uses it
    	// BadManufacturer = 2,
    	RemoteManufacturer = 3,
    	GoodManufacturer = 4
    };

With compile-time reflection available on the enum, one can simply write:

.. code-block:: cpp

    indexed_array<Configuration, MotorType> defaultConfiguration = /* initialize */;

This declare an array whose size is **3**:

.. code-block:: cpp
    :caption: The following guarantees hold

    assert(&defaultConfiguration[MotorType::LocalManufacturer] == begin(defaultConfiguration))
    assert(&defaultConfiguration[MotorType::RemoteManufacturer] == begin(defaultConfiguration) + 1);
    assert(&defaultConfiguration[MotorType::GoodManufacturer] == begin(defaultConfiguration) + 2);

If the enum has no compile-time reflection available, we can still use it:

.. code-block:: cpp
    :caption: Declaration using a value_sequence

    using MotorTypeList = value_sequence<MotorType,
        MotorType::LocalManufacturer,
        MotorType::RemoteManufacturer,
        MotorType::GoodManufacturer>;
    indexed_array<Configuration, MotorTypeList> defaultConfiguration = /* ... */;

The two arrays are identical.

Union of values
^^^^^^^^^^^^^^^

Sometimes we're interested in a subset of the values, which may also be non-contiguous. Let's
take back our example from the lift industry:

.. code-block:: cpp
    :caption: Definition of the Floor enum

    enum class Floor : uint8_t {
    	Car = 0,
    	F1 = 1,
    	F2 = 2,
    	F3 = 3,
    	... // the norm defines up to 254 Floors
    	F254 = 254,
    	All = 255
    };

In real life, no lift ever has so many floors. We only use a subset of the floors, for example 
``[F1..F32]`` or ``[F1..F64]``. We already covered that case earlier, using an index range. But lifts
also have some special *floors*, such as the machinery or the pit (beneath the lowest user level).
So, we modify our enum accordingly:

.. code-block:: cpp
    :caption: Slightly modified enum

    	/* previous values unchanged */
    	Machinery = 240,
    	Pit = 241,
    	/* ... other values are reserved for extensions */
    	All = 255
    };

We have some fire detectors, whose state (``Off``, ``On``, ``Unknown``) we must store in our program.
There is a detector at each floor, in machinery and in pit. But there is none inside the car. To
store this data, we can declare the following:

.. code-block:: cpp
    :caption: Array declaration using an union of values

    indexed_array<FiredetectorState, 
                  union_of<
                      index_range<Floor::F1, Floor::F32>,
                      single_value<Floor::Machinery>,
                      single_value<Floor::Pit>>> firedetectors;

This declares an ``indexed_array`` which contains **34** items (32 common floors + 2 technical ones). All
items are contiguous in memory and there is no room wasted:
``sizeof(firedetectors) == 34 * sizeof(FiredetectorState)``

The layout is the following:

+-----+-----+-----+-----+------+------+------+------+
|  0  |  1  |  2  | ... |  30  |  31  |  32  |  33  |
+-----+-----+-----+-----+------+------+------+------+
| VF1 | VF2 | VF3 | ... | VF31 | VF32 | VMac | VPit |
+-----+-----+-----+-----+------+------+------+------+

Name aliases
^^^^^^^^^^^^

Sometimes enums are defined this way, to provide poor-man's reflection:

.. code-block:: cpp
    :caption: Substitute for correct reflexion

    enum class MyEnum
    {
    	first,
    	AValue = first,
    	Another,
    	Foo,
    	last = Foo// if adding new values, update this
    };
    
    // This allows us to declare:
    indexed_array<int, index_range<MyEnum::first, MyEnum::last>> arr;
    assert(arr.size() == 3);

Lambda indexing
---------------

*This feature requires the use of a ``C++20`` compiler*.

To help reducing the need to write a custom indexer, the library provides a generic indexer, that
is parametrized by a size and a lamdba function. The provide lamda function is in charge of
taking a value of the type used for indexing, and return a `size_t` in the range ``[0, size - 1]``.

.. code-block:: cpp
    :caption: Example lambda indexer

    constexpr auto wday_indexing_l = [](std::chrono::weekday wd) { return static_cast<std::size_t>(wd.iso_encoding() - 1); };
    using wday_indexer = lambda_indexer<wday_indexing_l, 7>;
    indexed_array<std::string, wday_indexer> french_names{"lundi", "mardi", "mercredi", "jeudi", "vendredi", "samedi", "dimanche"};

The same can be achieved using a custom indexer with ``C++17``:

.. code-block:: cpp
    :caption: Custom indexer

    struct wday_indexer
    {
    	using index = std::chrono::weekday;
    	static constexpr std::size_t size = 7u;
    	template <bool b>
    	static constexpr std::size_t at(std::chrono::weekday wd)
    	{
    		auto v = wd.iso_encoding();
    		if constexpr (b)
    		{
    			if (v < 1 || v > 7)
    				throw std::out_of_range("Invalid value");
    		}
    		return static_cast<std::size_t>(v - 1);
    	}
    	static constexpr bool in_range(std::chrono::weekday wd)
    	{
    		auto v = wd.iso_encoding();
    		return v > 0 && v <= 7;
    	}
    };


Custom indexes
--------------

If needed, the library user can provide its own indexing mechanism. An indexer
must provide the following members:

* an ``index`` typedef, which is the type used in the public interface
* a static ``std::size_t size`` member, which is the size of the container
* a static ``template<bool b> size_t at(index)``, which does the actual
  translation between interface index and internal container index. The boolean is
  here to distinguish between throwing (``b`` is ``true``)/ non-throwing (``b`` is ``false``)
  versions. The indexer shall raise an ``out_of_range`` exception when ``b`` is ``true`` and
  it is given an invalid index. This method may be ``noexcept(!b)``.
* a static ``is_o1`` boolean value, which tells if the indexer is ``O(1)``. This is optional,
  but is provided by standard indexers, and may be used to optimize some algorithms.
* a static ``in_range(index)`` boolean method, which returns true if the given index
  is in the range of possible values (ie, ``at(index)`` won't throw).

This is an unrealistic example of a custom index:

.. code-block:: cpp
    :caption: Reverse fibonnaci indexer

    struct reverse_fibonnaci_index
    {
    	using index = int;
    	inline static constexpr size_t const size = 5;
    	template <bool b>
    	static constexpr auto at(index v) noexcept(!b)
    	{
    		if (v == 1)
    			return 0;
    		if (v == 2)
    			return 1;
    		if (v == 3)
    			return 2;
    		if (v == 5)
    			return 3;
    		if (v == 8)
    			return 4;
    		if constexpr(b)
    			throw std::out_of_range("invalid index");
    		return -1; // this value may be catched by other tools when accessing underlying array
    	}
    	static constexpr bool is_o1 = false;
    	static constexpr bool in_range(index v) noexcept
    	{
    		return at<false>(v) != -1;
    	}
    };
    
    // Which can then be used as:
    indexed_array<std::string, reverse_fibonnaci_index> data; // data.size() == 5

In real life, a reason to write a custom index is performance. Let's take back our lift floor example.
Accessing an element inside the container is now ``O(n)``, due to the way the indexer works. But it could be
made faster, something like:

.. code-block:: cpp
    :caption: Example of optimization by writing a custom indexer

    template <bool b>
    static constexpr auto at(index floor) noexcept(!b)
    {
    	if (v == Floor::Machinery)
    		return size() - 2;
    	if (v == Floor::Pit)
    		return size() - 1;
    	if constexpr (b)
    	{
    		if(floor >= Floor::F1 && floor <= Floor::F32)
    			return static_cast<size_t>(floor) - 1;
    		throw std::out_of_range("invalid index");
    	}
    	return static_cast<size_t>(floor) - 1;

Heterogeneous indexing
----------------------

*This feature relies on lambda indexing, and as such a ``C++20`` compiler is required.*

Heterogeneous indexing means using different types to access the same array. Two use cases are
supported:

* aliasing, ie using two different types to access the same area of the array. This can be seen
  as a way to do compile-time multi-indexing (for example, accessing with both a numeric index or a
  string identifier)
* slicing, ie using different types to access different sections of the same underlying array.

Taking back our example with from the lift industry, we could rewrite it like this:

.. code-block:: cpp
    :caption: Example of heterogeneous indexing

    // don't alter the Floor enum. Instead, define a new enum holding our special values
    enum SpecialFloors
    {
    	Machinery = 0,
    	Pit
    };
    
    struct LiftFloorIndexer
    {
    	// we put special floors at start, so we can handle any number of floors
    	std::size_t operator()(DS417::Floor c) 
    	{
    		// +1 because Floor starts at 1, +2 special floors, -1 for floor offset
    		return static_cast<std::size_t>(c) + 1; 
    	}
    	std::size_t operator()(SpecialFloors c)
    	{
    		return static_cast<std::size_t>(c);
    	}
    };
    
    using floors_indexer = lamdba_indexer<LiftFloorIndexer{}, 34>; // supports 2 special floors + 32 floors
