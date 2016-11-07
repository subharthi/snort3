/*
 * The rolling_sampler accumulator uses reservoir sampling to store a random sample of the data
 * It attempts to gather samples of data from a rolling window instead of over the entire data set
 * instead of new samples replacing any sample 
 */

#include <vector>
#include <ctime>

#include <boost/mpl/placeholders.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>
#include "statistics/count.hpp"
//#include <boost/accumulators/statistics/count.hpp>
#include <boost/range/iterator_range.hpp>

#include <boost/parameter/keyword.hpp>

#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <boost/circular_buffer.hpp>


namespace boost { namespace accumulators {

BOOST_PARAMETER_KEYWORD(tag, rolling_sampler_sample_size)

namespace impl {

    template<typename Sample>
    struct rolling_sampler_impl
      : accumulator_base
    {
        typedef typename circular_buffer<Sample>::const_iterator const_iterator;
        typedef iterator_range<const_iterator> result_type;

        template<typename Args>
        rolling_sampler_impl(Args const & args) :
           samples(args[rolling_sampler_sample_size]),
           max_samples(args[rolling_sampler_sample_size]),
           rolling_window_size(50)
        {
            // TODO samples should reserve max_samples
        }

        template<typename Args>
        void operator ()(Args const & args)
        {
            if(samples.size() < max_samples) {
                this->samples.push_back(args[sample]);
            } else {
                std::size_t cnt = count(args);
                if( cnt > rolling_window_size ) {
                    cnt = rolling_window_size;
                }
                int rnum = boost::random::uniform_int_distribution<long unsigned int>{0, cnt}(gen);
                if(rnum < samples.size()) {
                    this->samples.push_back(args[sample]);
                }
            }
        }

        result_type result(dont_care) const
        {
            return result_type(this->samples.begin(), this->samples.end());
        }
    private:
        // TODO this should be seeded with something
	// FIXED: staghavi@cisco Produces randomness out of thin air 
	std::time_t now = std::time(0);
        boost::mt19937 gen{static_cast<std::uint32_t>(now)};
        int max_samples;
        int rolling_window_size;
        circular_buffer<Sample> samples;
    };

}

namespace tag {
    struct rolling_sampler 
        : depends_on<count>,
          rolling_sampler_sample_size {
        typedef typename accumulators::impl::rolling_sampler_impl<mpl::_1> impl;
    };
}

namespace extract {
    extractor<tag::rolling_sampler> const rolling_sampler = {};

    BOOST_ACCUMULATORS_IGNORE_GLOBAL(rolling_sampler)
}

using extract::rolling_sampler;


}}

