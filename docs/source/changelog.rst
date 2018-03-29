.. Copyright (c) 2017, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Changelog
=========

0.4.5
-----

- xget for variant on xclosure_wrapper

0.4.4
-----

- bug fix in any
- hierarchy generators

0.4.3
-----

- missing near integers functions for `xoptional`
- `xoptional` compilation issue fixed

0.4.2
-----

- added missing operators for xoptional
- removed compiler warning if cpp_exceptions already defined

0.4.1
-----

- Bug fix in move semantics for xoptional free functions (`value` and `has_value`)
- Use `static_if` instead of regular `if` to remove gcc-6 warning.
- Document installation with the Spack package manager.
- Fix complex operators with closure wrappers.
- Integrate upstream fix for the variant implementation.

0.4.0
-----

- Migration to modern target-based cmake

0.3.9
-----

- Bug fix in the computing of hashes for 32 bit platforms
- Fixing warnings

0.3.8
-----

- Improvements and fixes in base iterators (common iterator tag)

0.3.7
-----

- Fixes in `xoptional`.

0.3.6
-----

- Addition of base iterators for linear containers, and associative containers.

0.3.5
-----

- Addition of `value` and `has_value` free functions.
- Bug fix in comparison operator for `xclosure_wrapper`.

0.3.4
-----

- Better semantics for assignment operators in `xoptional`.
- Addition of `static_if` in `xtl::mpl`.
- Addition of `xtl::identity` functor in xfunctional.

0.3.3
-----

- Work around Visual Studio compiler bug in `xoptional_proxy`.

0.3.2
-----

- Improvement of xoptional value semantics (explicit constructors when underlying value type not implicitely constructable)

0.3.1
-----

- Fixes in closure wrapper semantics

0.3.0
-----

- Improve optional sequence
- Use dynamic bitset in optional vector
- Added base64encode and base64decode

0.2.11
------

- Added dynamic bitset

0.2.10
------

- Added meta programming tools

0.2.9
-----

- Added variant implementation

0.2.8
-----

- Added proxy wrapper for pointer semantics.

0.2.7
-----

- Added implementation for closure pointer

0.2.6
-----

- Added base class for random access iterators

0.2.5
-----

- Added closure wrappers

0.2.4
-----

- Added implementation of std::any

0.2.3
-----

- Fixed bug in fixed-size string hashing

0.2.2
-----

- Added the hashing of fixed-size strings

0.2.1
-----

- Fixed-size strings
- Fixup issue with ambiguous overload of operator<<

0.2.0
-----

- Moving features from xtensor (xcomplex, xoptional, xsequence, xtypetraits)
