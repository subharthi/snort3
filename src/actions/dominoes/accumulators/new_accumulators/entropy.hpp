/*
 * Calculating the entropy of the data that is passed in
 */
#include "sampler.hpp"

namespace boost { namespace accumulators {

// I believe last I checked this doesn't work correctly
// TODO write entropy calculator that uses sampling
// TODO write entropy calculator using data stream
namespace impl {

    template<typename Sample>
    struct entropy_impl
      : accumulator_base
    {
        typedef double result_type;

        entropy_impl(dont_care) { }

        void operator ()(dont_care) { }

        template<typename Args>
        result_type result(Args const & args) const
        {
            auto samples = sampler(args);
            double entropy = 0;
            double max_entropy = 0;

            std::map<Sample, int>frequency;
            for( auto &iter : samples ) {
                frequency[iter]++;
            }

            for( auto &iter : frequency ) {
                double prob = ((double) iter.second) / samples.size();
                entropy += prob * log2(prob);
            }
            entropy = entropy * -1;

            max_entropy = log2(frequency.size());

            return entropy;
        }
    };

}

namespace tag {
    struct entropy 
        : depends_on<sampler> {
        typedef typename accumulators::impl::entropy_impl<mpl::_1> impl;
    };
}

namespace extract {
    extractor<tag::entropy> const entropy = {};

    BOOST_ACCUMULATORS_IGNORE_GLOBAL(entropy)
}

using extract::entropy;


}}

