/***************************************************************************
* Copyright (c) 2016, Sylvain Corlay and Johan Mabille                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTL_XITERATOR_BASE_HPP
#define XTL_XITERATOR_BASE_HPP

#include <cstddef>
#include <iterator>

namespace xtl
{
    template <class I, class T, class D = std::ptrdiff_t, class P = T*, class R = T&>
    class xrandom_access_iterator_base
    {

    public:

        using derived_type = I;
        using value_type = T;
        using reference = R;
        using pointer = P;
        using difference_type = D;
        using iterator_category = std::random_access_iterator_tag;

        inline friend derived_type operator++(derived_type& d, int)
        {
            derived_type tmp(d);
            ++d;
            return tmp;
        }

        inline friend derived_type operator--(derived_type& d, int)
        {
            derived_type tmp(d);
            --d;
            return tmp;

        }

        inline reference operator[](difference_type n) const
        {
            return *(*this + n);
        }

        inline friend derived_type operator+(const derived_type& it, difference_type n)
        {
            derived_type tmp(it);
            return tmp += n;
        }

        inline friend derived_type operator+(difference_type n, const derived_type& it)
        {
            derived_type tmp(it);
            return tmp += n;
        }

        inline friend derived_type operator-(const derived_type& it, difference_type n)
        {
            derived_type tmp(it);
            return tmp -= n;
        }

        inline friend bool operator!=(const derived_type& lhs, const derived_type& rhs)
        {
            return !(lhs == rhs);
        }

        inline friend bool operator<=(const derived_type& lhs, const derived_type& rhs)
        {
            return !(rhs < lhs);
        }

        inline friend bool operator>=(const derived_type& lhs, const derived_type& rhs)
        {
            return !(lhs < rhs);
        }

        inline friend bool operator>(const derived_type& lhs, const derived_type& rhs)
        {
            return rhs < lhs;
        }
    };

    template <class T>
    using xrandom_access_iterator_base2 = xrandom_access_iterator_base<typename T::iterator_type,
                                                                       typename T::value_type,
                                                                       typename T::difference_type,
                                                                       typename T::pointer,
                                                                       typename T::reference>;
}

#endif
