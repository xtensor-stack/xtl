.. Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Basic types
===========

any
---

`xtl::any` is a backport of the C++17 class `std::any`. The class describes a 
type-safe container for single values of any type:

.. code::

    #include <iostream>
    #include "xtl/xany.hpp"

    xtl::any a = 1;
    std::cout << a.type().name() << ": " << xtl::any_cast<int>(a) << std::endl;
    // => i: 1

    try
    {
        std::cout << xtl::any_cast<float>(a) << std::endl;
    }
    catch(const xtl::bad_any_cast& e)
    {
        std::cout << e.what() << std::endl;
    }
    // => bad any_cast

    a.reset();
    std::cout << std::boolalpha << a.empty() << std::endl;
    // => true

The API of `xtl::any` is the same as that of `std::any`. Full documentation
can be found on `cppreference <https://en.cppreference.com/w/cpp/utility/any>`_.

xbasic_fixed_string
-------------------

TODO

xcomplex
--------

`xcomplex` is the equivalent to `std::complex`, where the real ang imaginary
part can be stored either as values, or as references. Therefore, it can be
used as a proxy on real values already initialized. This is particularly
interesting for storing real and imaginary parts in different containers,
and gather them as complex values for computation. This allows optimzations
(such as vectorization) on the real and imgaginary values.

.. code::

    #include <iostream>
    #include <vector>
    #include "xtl/xcomplex.hpp"

    std::vector<double> arg1_real = { 1., 2.};
    std::vector<double> arg1_imag = { 3., 4.};
    std::vector<double> arg2_real = { 2., 4.};
    std::vector<double> arg2_real = { 1., 3.};
    std::vector<double> res_real(2);
    std::vector<double> res_imag(2);

    using complex = xtl::xcomplex<double&, double&>;
    using const_complex = xtl::xcomplex<const double&, const double&>;
    for (size_t i = 0; i < 2; ++i)
    {
        complex res(res_real[i], res_img[i]);
        res = const_complex(arg1_real, arg1_imag) * const_complex(arg2_real, arg2_imag);
        std::cout << "res = (" << res.real(), << ", " << res.imag() << std::endl;
    }

The API of `xtl::xcomplex` is the same as that of `std::complex`, with the ability
to store values as references. Full documentation can be found on
`cppreference <https://en.cppreference.com/w/cpp/numeric/complex>`_.

half_float
----------

The `half_cloat` class implements an IEEE-conformant half-precision floating-point type
with the usual arithmetic operators and conversions. It is implicitly convertible from
single-precision floating-point, which makes expressions and fnuctions with mixed-type
operands to be of the most precise operand type.

.. code::

    #include <iostream<
    #include "xtl/xhalf.hpp"

    xtl::half_float f0 = 1.0f;
    xtl::half_float f1 = 2.0f;
    auto res = f0 + f1;
    std::cout << res << std::endl;

xmasked_value
-------------

TODO

xoptional
---------

TODO

xvariant
--------

TODO

