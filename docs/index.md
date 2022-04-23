<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Overview

This library provides a generic fixed-size container, that is a drop-in
replacement for `std::array`, that uses custom indexes. It allows writing
code like the following:
```cpp
enum class Foo { Bar1 = 4, Bar2, Bar3 };
// ...
indexed_array<string, Foo> foo; // foo.size() == 3
foo[Foo::Bar2] = "an example string"; // modify the second element of foo
```

Multi-dimensional indexers are supported as well, you can write something like

```cpp
indexed_array<string, Foo, Foo> foo; // foo.size() == 9
foo.at(Foo::Bar2, Foo::Bar2) = "an example string"; // modify the “central” element
// will be supported when c++23 is out
// foo[Foo::Bar2, Foo::Bar2] = "an example string";
```

See [Basic Usage](basicusage.md) for information on how to use the library.

It also provide *safe initialization*, a compile time check that can be used
to detect code errors, and comes in very handy to detect breakage when on enum
value changes. See [Safe Initialization](safeinitialization.md).

To learn how to write a custom indexer, see [Custom Indexer](customindexer.md).

If you're worried about performance, see [Performance Considerations](performance.md).

See [Design Principles](design.md) for details about the internals of the library.

For a comparison with other ways to achieve similar results, see [Comparison](comparison.md)

And finally, the [Reference](reference.md).
