//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_FILL_HPP
#define RANGES_V3_ALGORITHM_FILL_HPP

#include <utility>
#include <algorithm>
#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct filler : bindable<filler>
        {
            /// \brief template function \c filler::operator()
            ///
            /// range-based version of the \c fill std algorithm
            ///
            /// \pre \c ForwardRange is a model of the ForwardRange concept
            template<typename ForwardRange, typename Value>
            static ForwardRange invoke(filler, ForwardRange && rng, Value const & val)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                std::fill(ranges::begin(rng), ranges::end(rng), val);
                return std::forward<ForwardRange>(rng);
            }

            /// \overload
            template<typename Value>
            static auto invoke(filler fill, Value && val)
                -> decltype(fill.move_bind(std::placeholders::_1, std::forward<Value>(val)))
            {
                return fill.move_bind(std::placeholders::_1, std::forward<Value>(val));
            }
        };

        RANGES_CONSTEXPR filler fill {};

    } // inline namespace v3
}

#endif // include guard
