/*
 * The topk accumulator is used to determine the k most frequently input values
 */
#ifndef __TOPK_HPP__
#define __TOPK_HPP__

#include <algorithm>
#include <vector>
#include <map>

#include <boost/mpl/placeholders.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include "unique.hpp"

#include <boost/parameter/keyword.hpp>

// TODO there are probably more efficient ways of doing this
// TODO create a probabilistic version of this

namespace boost { namespace accumulators {

namespace impl {

    template<typename Sample>
    struct topk_impl
      : accumulator_base
    {
        typedef typename std::pair<Sample, int> sample_freq;
        typedef typename std::vector<Sample> result_type;

        template<typename Args>
        topk_impl(Args const & args) :
            count(10)
        {
        }

        template<typename Args>
        void operator ()(Args const & args)
        {
        }

        template<typename Args>
        result_type result(Args const & args) const
        {
            auto unique_values = unique(args);
            std::vector<sample_freq> unique_vec;
            for (auto&& i : unique_values) unique_vec.push_back(i);
            std::sort(unique_vec.begin(), unique_vec.end(), 
                      [](sample_freq a, sample_freq b) { return a.second > b.second; });
            std::vector<Sample> topk_values;
            auto iter = unique_vec.begin();
            for(int i = 0; iter != unique_vec.end() && i < count; ++iter, ++i) {
                topk_values.push_back(iter->first);
            }

            // TODO topk_values should only have the key not the value
            return topk_values;
        }
    private:
        int count;
    };

}

namespace tag {
    struct topk 
        : depends_on<unique> {
        typedef typename accumulators::impl::topk_impl<mpl::_1> impl;
    };
}

namespace extract {
    extractor<tag::topk> const topk = {};

    BOOST_ACCUMULATORS_IGNORE_GLOBAL(topk)
}

using extract::topk;


}}

#endif
