/// \file
// Range v3 library
//
// Copyright Eric Niebler 2004
// Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#ifndef RANGES_V3_NUMERIC_ADJACENT_DIFFERENCE_HPP
#define RANGES_V3_NUMERIC_ADJACENT_DIFFERENCE_HPP

#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/algorithm/tagspec.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/unreachable.hpp>

namespace ranges
{
    inline namespace v3
    {
        CONCEPT_def
        (
            template(typename I, typename O, typename BOp = minus, typename P = ident)
            (concept AdjacentDifferentiable)(I, O, BOp, P),
                InputIterator<I> &&
                Invocable<P&, value_type_t<I>> &&
                CopyConstructible<uncvref_t<invoke_result_t<P&, value_type_t<I>>>> &&
                Movable<uncvref_t<invoke_result_t<P&, value_type_t<I>>>> &&
                OutputIterator<O, invoke_result_t<P&, value_type_t<I>>> &&
                Invocable<BOp&, invoke_result_t<P&, value_type_t<I>>, invoke_result_t<P&, value_type_t<I>>> &&
                OutputIterator<O, invoke_result_t<BOp&, invoke_result_t<P&, value_type_t<I>>, invoke_result_t<P&, value_type_t<I>>>>
        );

        struct adjacent_difference_fn
        {
            CONCEPT_template(typename I, typename S, typename O, typename S2,
                typename BOp = minus, typename P = ident)(
                requires Sentinel<S, I> && Sentinel<S2, O> &&
                    AdjacentDifferentiable<I, O, BOp, P>)
            tagged_pair<tag::in(I), tag::out(O)>
            operator()(I begin, S end, O result, S2 end_result, BOp bop = BOp{},
                       P proj = P{}) const
            {
                // BUGBUG think about the use of coerce here.
                using V = value_type_t<I>;
                using X = invoke_result_t<P&, V>;
                coerce<V> v;
                coerce<X> x;

                if(begin != end && result != end_result)
                {
                    auto t1(x(invoke(proj, v(*begin))));
                    *result = t1;
                    for(++begin, ++result; begin != end && result != end_result;
                        ++begin, ++result)
                    {
                        auto t2(x(invoke(proj, v(*begin))));
                        *result = invoke(bop, t2, t1);
                        t1 = std::move(t2);
                    }
                }
                return {begin, result};
            }

            CONCEPT_template(typename I, typename S, typename O, typename BOp = minus,
                typename P = ident)(
                requires Sentinel<S, I> &&
                    AdjacentDifferentiable<I, O, BOp, P>)
            tagged_pair<tag::in(I), tag::out(O)>
            operator()(I begin, S end, O result, BOp bop = BOp{}, P proj = P{}) const
            {
                return (*this)(std::move(begin), std::move(end), std::move(result),
                               unreachable{}, std::move(bop), std::move(proj));
            }

            CONCEPT_template(typename Rng, typename ORef, typename BOp = minus, typename P = ident,
                typename I = iterator_t<Rng>, typename O = uncvref_t<ORef>)(
                requires Range<Rng> &&
                    AdjacentDifferentiable<I, O, BOp, P>)
            tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
            operator()(Rng &&rng, ORef &&result, BOp bop = BOp{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), static_cast<ORef &&>(result), std::move(bop),
                               std::move(proj));
            }

            CONCEPT_template(typename Rng, typename ORng, typename BOp = minus,
                typename P = ident, typename I = iterator_t<Rng>,
                typename O = iterator_t<ORng>)(
                requires Range<Rng> && Range<ORng> &&
                    AdjacentDifferentiable<I, O, BOp, P>)
            tagged_pair<tag::in(safe_iterator_t<Rng>),
                tag::out(safe_iterator_t<ORng>)>
            operator()(Rng &&rng, ORng &&result, BOp bop = BOp{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), begin(result), end(result),
                               std::move(bop), std::move(proj));
            }
        };

        RANGES_INLINE_VARIABLE(adjacent_difference_fn, adjacent_difference)
    }
}

#endif
