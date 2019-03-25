.. Copyright (c) 2017, Johan Mabille and Sylvain Corlay

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Build and configuration
=======================

Build
-----

``xtl`` build supports the following options:

- ``BUILD_TESTS``: enables the ``xtest`` target (see below).
- ``DOWNLOAD_GTEST``: downloads ``gtest`` and builds it locally instead of using a binary installation.
- ``GTEST_SRC_DIR``: indicates where to find the ``gtest`` sources instead of downloading them.
- ``XTL_DISABLE_EXCEPTIONS``: indicates that tests should be run with exceptions disabled.

All these options are disabled by default. Enabling ``DOWNLOAD_GTEST`` or setting ``GTEST_SRC_DIR``
enables ``BUILD_TESTS``.

If the ``BUILD_TESTS`` option is enabled, the following target is available:

- xtest: builds an run the test suite.

For instance, building the test suite of ``xtl`` where the sources of ``gtest`` are located in e.g. ``/usr/share/gtest``:

.. code::

    mkdir build
    cd build
    cmake -DGTEST_SRC_DIR=/usr/share/gtest ../
    make xtest
