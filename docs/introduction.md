<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# The problem

In C++, which inherited this feature from C, arrays (both C arrays and `std::array`s)
are indexed by unsigned integer values (`std::size_t`), starting at 0. This gives us
information about the valid values for indexing, but this gives a very little information
about **what** (ie, the semantic) is actually used as the index. Is it a day, a number of
items, a pin identifier, a floor in a building?

This is also error prone, as it is easy to inadvertently use an incorrect value for
the index. While the use of `.at()` can detect it at run-time, it would be better if
we could diagnose it at compile time.

Last, but not least, not every value set starts at 0. This is an extract of the linux documentation:

```
struct tm {
    int tm_sec;    /* Seconds (0-60) */
    int tm_min;    /* Minutes (0-59) */
    int tm_hour;   /* Hours (0-23) */
    int tm_mday;   /* Day of the month (1-31) */
    int tm_mon;    /* Month (0-11) */
    int tm_year;   /* Year - 1900 */
    int tm_wday;   /* Day of the week (0-6, Sunday = 0) */
    int tm_yday;   /* Day in the year (0-365, 1 Jan = 0) */
    int tm_isdst;  /* Daylight saving time */
};
```

You can see that we have, in the same structure, most values starting at 0, but the day of the
month starting at 1. Such inconsistencies are common accross software, and may be at the root of
some subtle bugs. Some things are either inherently, or due to some external specification,
starting at 1 (or at another arbitrary value, -1 comes to mind), and using them as an array index
means applying the offset everywhere.

Another example comes from the lift industry. Floors are, in the DS417 specification, defined from
1 to 254, 0 being reserved to indicate the car (255 indicating all floors). If we are storing the
list of active calls for every floor in a table, it is much more convenient to use 1-based indexing
than 0-based, for consistency accross the program.

# The old solution: unscoped enums (or defines)

To add semantic, we can since a long time use unscoped enums instead of integers.

```
enum day_of_week { sunday, monday, tuesday, wednesday, thursday, friday, saturday };
enum month { january, february, march, april, may, june, july, august, september, october, november, december };
```

Now we can write:
```
std::array<schedules, 7> recurring_schedules; // 7 days in a week. Hard-coded constant, not terrible

/* ... */

recurring_schedules[sunday].add(...);
```

That's better. But we can also write:

```
recurring_schedules[january].add(...); // oops
```

For which we would like a compile-time error. And if we need an offset, we'll still need to apply it
manually everywhere, which may be easy to forget.

# Enum classes: it gets worse

C++11 has introduced enum class to give type safety to enum. So let's try to use that to solve our problem.

```
enum day_of_week { sunday, monday, tuesday, wednesday, thursday, friday, saturday };
enum month { january, february, march, april, may, june, july, august, september, october, november, december };
```

But now we need to write:
```
// recurring_schedules[day_of_week::sunday].add(...); // Does not compile
recurring_schedules[static_cast<std::size_t>(day_of_week::sunday).add(...); // OK

recurring_schedules[static_cast<std::size_t>(month::january)].add(...); // Ooops, still compiles fine
```

So, basically, what we got is that it is longer to write, and since we need to cast everything to a
`size_t` at the end we don't gain any type-safety. This makes the code actually worse than with
unscoped enums, for no benefit.

A common solution people resort to is to use a `map<day_of_week, schedule>`. This may be ok, but the 
strucure is completely different from an array, requires dynamic allocation, does not guarantee data
locality and contiguity, etc. While this may be an affordable option in a lot of scenarios, there is
a whole range of others where it is not.

# A search for a better solution

What we ideally need is the following:
```
indexed_array<schedules, day_of_week> recurring_schedules;

/* ... */

recurring_schedules[day_of_week::sunday].add(...); // OK
// recurring_schedules[month::january].add(...); // Fails to compile
```

The benefits are visible:

* the code is just a bit longer to write than with unscoped enum
* the code is a lot shorter than with scoped enums and std::array
* it is much more difficult to inadvertently use a wrong value to access the array content (this is not
a guarantee: a scoped enum can contain any value of the underlying type, so bound checking must still be
an option).
* as a bonus, we want to handle offsets automatically (if `day_of_week` starts at 1, we dont have to
substract this offset on every call)

## The challenges

We can identify the following challenges:

* we need some way to retrieve the number of values in the enums at compile time
* not all enums starts at 0, and not all enums are a set of contiguous values
* there should be no run-time overhead over using a plain `std::array`, in the case 
we are contiguous and start at 0
* sometimes using the whole enum range is not desired. Sub-ranges should be supported.

# How it works

`indexed_array` has been designed to address these issues.
The library in itself does not provide enum reflection. It relies on others (such as *boost describe*,
*wise enum* or *magic enum*) to provide the following information:

* a compile time list of all enum values
* a corresponding compile time list of corresponding integer values

What it provides is a generic *array-like* container, whose size is given by an *indexer*, which is
also in charge of translating a *public* index (which would be an enum, but can be nearly anything)
 into a *private* index (which is the index of the item in the underlying array, an unsigned integer).

`indexed_array<schedule, day_of_week>` is an alias for `detail::indexed_array<schedule, indexer<day_of_week>>`
and will actually have the following underlying array: 
`array<schedule, indexer<day_of_week>::size>` where the implementation of `size`
depends on the enum library used. And the call to `operator[](day_of_week d)` will call
`indexer<day_of_week>::at(d)` whose implementation will return the index of `d` in the list
of all enum values.

Using this scheme, we will be able to support:

* enum values
* arbitrary ranges (such as `[-3..5]`)
* holes (ie, non-contiguous sets of values) should be supported as well
* even multidimensional arrays are possible. They can be seen as an array indexed by a tuple of values

All that is needed is to provide the appropriate indexer class. The library provides one, which is
suitable for most use-cases, and the user can provide its own for any domain-specific use case.

Back to [index](index.md), or continue with the [tutorial](tutorial.md)
