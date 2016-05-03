/*
 * The unique accumulator is used to track the count of unique objects that was input
 * there will be both a probablistic and a non-probabalistic version of it
 * TODO use a bool vector to track the exact matches of a value
 */
#ifndef __UNIQUE_HPP__
#define __UNIQUE_HPP__

#include <map>

#include <boost/mpl/placeholders.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>
#include <boost/accumulators/statistics/count.hpp>

#include <boost/parameter/keyword.hpp>


// TODO add possible variants
// probabalistic
// exact
namespace boost { namespace accumulators {

// The number of bins can be used to vary accuracy vs space
BOOST_PARAMETER_NESTED_KEYWORD(tag, hyperloglog_bins, bins)

namespace impl {

    template<typename Sample, typename Variant>
    struct unique_impl
      : accumulator_base
    {
        BOOST_MPL_ASSERT((
            mpl::or_<is_same<Variant, probabalistic>, is_same<Variant, exact> >
        ));

        typedef typename std::map<Sample, int> result_type;

        template<typename Args>
        unique_impl(Args const & args)
        {
        }

        template<typename Args>
        void operator ()(Args const & args)
        {
            this->unique_count[args[sample]]++;
        }

        result_type result(dont_care) const
        {
            return this->unique_count;
        }
    private:
        // TODO use a sparse boolean vector
        std::map<Sample, int> unique_count;
    };

}

namespace tag {
    struct unique 
        : depends_on<> {
        typedef typename accumulators::impl::unique_impl<mpl::_1> impl;
    };
}

namespace extract {
    extractor<tag::unique> const unique = {};

    BOOST_ACCUMULATORS_IGNORE_GLOBAL(unique)
}

using extract::unique;


}}

#endif
