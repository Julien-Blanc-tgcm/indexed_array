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

# Table of content

* [Introduction](introduction.md) for the rationale of the library.
* [Tutorial](tutorial.md) for simple usage
* [Advanced usage](advancedusage.md)
* [Multidimensional array](multidimensional.md)
* [Safe initialization](safeinitialization.md)
* [Performance considerations](performance.md)
* [Integrations and extensions](extensions.md)
* [Reference](reference.md)
* [Design principles](design.md)
* [Comparison with other libraries](comparison.md)

