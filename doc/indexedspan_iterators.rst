.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

indexed_span iterators
======================

.. _indexed_span_begin:

begin
-----

.. cpp:function:: constexpr iterator indexed_span::begin() const
.. cpp:function:: constexpr const_iterator indexed_span::cbegin() const

end
---

.. _indexed_span_end:

.. cpp:function:: constexpr iterator indexed_span::end()
.. cpp:function:: constexpr const_iterator indexed_span::cend() const

rbegin
------

.. _indexed_span_rbegin:

.. cpp:function:: constexpr reverse_iterator rbegin()
.. cpp:function:: constexpr const_reverse_iterator crbegin() const

rend
----

.. _indexed_span_rend:

.. cpp:function:: constexpr reverse_iterator rend()
.. cpp:function:: constexpr const_reverse_iterator crend() const

    Standard iteration. Same semantic as the corresponding methods in ``std::span``.

Multidimensional
----------------

.. warning::

    For multidimensional spans, it must be remembered that iteration is done through
    the whole content, and not a single dimension.
