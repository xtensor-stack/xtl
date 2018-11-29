#ifndef XTL_JSON_HPP
#define XTL_JSON_HPP

#include <cstddef>
#include <string>

#include "nlohmann/json.hpp"

#include "xoptional.hpp"
#include "xbasic_fixed_string.hpp"

namespace xtl
{
    /***********************************************************
     * to_json and from_json specialization for xtl::xoptional *
     ***********************************************************/

    template <class D>
    void to_json(nlohmann::json& j, const xoptional<D>& o)
    {
        if (!o.has_value())
        {
            j = nullptr;
        }
        else
        {
            j = o.value();
        }
    }

    template <class D>
    void from_json(const nlohmann::json& j, xoptional<D>& o)
    {
        if (j.is_null())
        {
            o = missing<D>();
        }
        else
        {
            o = j.get<D>();
        }
    }

    template <class CT, std::size_t N, int ST, template <std::size_t> class EP, class TR>
    void to_json(::nlohmann::json& j, const xbasic_fixed_string<CT, N, ST, EP, TR>& str)
    {
        j = str.c_str();
    }

    template <class CT, std::size_t N, int ST, template <std::size_t> class EP, class TR>
    void from_json(const ::nlohmann::json& j, xbasic_fixed_string<CT, N, ST, EP, TR>& str)
    {
        str = j.get<std::string>();
    }
}

#endif
