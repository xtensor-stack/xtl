.. Copyright (c) 2017, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

.. image:: xtl.svg
   :alt: xtl

Basic tools (containers, algorithms) used by other quantstack packages

Introduction
------------

`xtl` gathers generic purpose algorithms and containers that are used by
the `xtensor` stack and the `xeus` stack.

Some of the features are C++14 backport of C++17 classes and algorithms,
such as `variant` or `any`.

Licensing
---------

We use a shared copyright model that enables all contributors to maintain the
copyright on their contributions.

This software is licensed under the BSD-3-Clause license. See the LICENSE file for details.

.. toctree::
   :caption: INSTALLATION
   :maxdepth: 2

   installation
   changelog

.. toctree::
   :caption: USAGE
   :maxdepth: 2

   basic_types
   containers
   meta_programming
   design_patterns
   miscellaneous

.. toctree::
   :caption: DEVELOPER ZONE

   build-options
   releasing
