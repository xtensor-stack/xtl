/***************************************************************************
* Copyright (c) 2017, Sylvain Corlay and Johan Mabille                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTL_CONFIG_HPP
#define XTL_CONFIG_HPP

#define XTL_VERSION_MAJOR 0
#define XTL_VERSION_MINOR 6
#define XTL_VERSION_PATCH 1

// Attempt to discover whether we're being compiled with exception support
#ifndef XTL_NO_EXCEPTIONS
#if !(defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND))
#define XTL_NO_EXCEPTIONS
#endif
#endif

#endif
