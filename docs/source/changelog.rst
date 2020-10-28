.. Copyright (c) 2017, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Changelog
=========

0.6.21
------

- CMake: Modernized GTest integration
- Add support for ``xtl::endianness``

0.6.20
------

- Added xtraits for future simple specialization
- Added half_float implementation

0.6.19
------

- Improved ``mpl::contains`` implementation
- Added ``are_equivalent_sequences``
- Various minor improvements

0.6.18
------

- Relaxed dimension constraint on multidispatcher
- Replaced throw with XTL_THROW to support disabling exceptions

0.6.17
------

- Implemented ``index_of`` for ``mpl::vector``
- Implemented multimethods pattern
- Implemented visitor pattern

0.6.16
------

- Fixed mpark variant inclusion guards
- Add a serialiser for xvariant to json
- Removed capture all by reference

0.6.15
------

- Renamed mpark variant header inclusion guard

0.6.14
------

- Implemented value iterator for map containers

0.6.13
------

- mpark/variant small change for CUDA 10.2 workaround
- Switched the documentation build to QuantStack channel
- Refactored CI

0.6.12
------

- NVCC CUDA compiler compatibility
- Wrapped call to ``find_package``

0.6.11
------

- Avoids C++20 "requires" keyword

0.6.10
------

- Set up xtl target's public headers
- CMake: adding C++14 standard to target

0.6.9
-----

- Implemented stepping iterators

0.6.8
-----

- Fixed murmur implementation for x86 platform

0.6.7
-----

- Specialized ``promote_type`` for ``std::complex``

0.6.6
-----

- Fixed ``promote_type`` for ``std::chrono::time_point``
- Update README for Conan installation instructions

0.6.5
-----

- Add supports for clang-cl compiler
- Fix cmake command
- Fix compiler error with clang-cl compiler

0.6.4
-----

- Fixed forward type

0.6.3
-----

- Fix constness issue in xtl's implementation of std::any.

0.6.2
-----

- Allows xtl to build with -fno-exceptions
- Added ``constify`` and ``constify_t``
- Added ``size_t`` overloads for random access iterators

0.6.1
-----

- Latex does not know how to include svg
- Added ``XTL_REQUIRES_IMPL`` macro
- Removed warnings

0.6.0
-----

- Standalone build of xtl tests
- Moved ``xmasked_value`` from `xtensor`
- Moved ``promote_type`` from `xtensor`
- Disabled ``xoptional`` methods for ``xmasked_value``
- Implemented ``select``
- ``make_sequence``  from ``initializer_list``

0.5.4
-----

- Implementation of mpl::unique
- Prevent installation of gtest artifact

0.5.3
-----

- upgraded to mpark/variant 1.4.0
- implemented concepts
- implemented split of type lists

0.5.2
-----

- fixed C++11 compatibility in xjson.hpp

0.5.1
-----

- reverse order of initialization of optional
- fixup mime type rendering of fixed string
- closure wrapper assignment fixed

0.5.0
-----

- Serialization and deserialization of fixed strings
- Inequality comparisons removed from bidirectional iterator base
- Simplified forward sequence
- Fixed forward sequence
- Removed warnings
- const reference getter for variant holding non const references
- xget on rvalue fixed
- Added storage option to fixed string
- Added missing entries of header files in CMakeLists.txt
- Refactored xdynamic_bitset
- Fixed forwrad sequence for non resizable types
- Removed meta pop-back

0.4.16
------

- meta find_if implementation
- Enable CTest and CMake cleanup
- Make nlohmann_json optional in the tests, exported C++14 requirements

0.4.15
------

- Value types in const closures are not const qualified anymore, to allow move
- Added third template parameter to forward_sequence that allows for true
  forwarding of sequences

0.4.14
------

- Fixed typo in 'xtl.pc.in'
- Removed -march=native from systems that do not support in CMakeLists
- Added hash.verification result for big-endian systemss
- Fixed common_optional_impl
- Implemented xeus-cling mime_bundle_repr for xoptional, xcomplex and xfixed_string

0.4.13
------

- CMake call to find_package with nlohmann_json is QUIET
- Fix typo in xoptional swap
- Added pkgconfig support

0.4.12
------

- operator overload fixes for xcomplex

0.4.11
------

- add missing `<limits>` header in xcomplex
- fix xcomplex isnan test

0.4.10
------

- `xcomplex` implementation
- `xcomplex_sequence` implementation

0.4.9
-----

- return type of `static_if` fixed

0.4.8
-----

- support for JSON serialization of xoptionals

0.4.7
-----

- support for uninitialized `make_sequence`

0.4.6
-----

- remove an unused file.
- support for overloaded lambdas

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
