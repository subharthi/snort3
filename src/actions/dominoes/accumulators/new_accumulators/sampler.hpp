/*
 * The sampler accumulator uses reservoir sampling to store a random sample of the data
 */

#include <vector>

#include <boost/mpl/placeholders.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>
#include <boost/accumulators/statistics/count.hpp>

#include <boost/parameter/keyword.hpp>

#include <boost/random/random_device.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

// TODO create weighted sampler

namespace boost { namespace accumulators {

BOOST_PARAMETER_KEYWORD(tag, sampler_sample_size)

namespace impl {

    template<typename Sample>
    struct sampler_impl
      : accumulator_base
    {
        typedef typename std::vector<Sample> result_type;

        template<typename Args>
        sampler_impl(Args const & args) :
           samples(args[sample | Sample()]),
           max_samples(args[sampler_sample_size])
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
                int rnum = boost::random::uniform_int_distribution<long unsigned int>{0, cnt}(gen);
                if(rnum < samples.size()) {
                    this->samples[rnum] = args[sample];
                }
            }
        }

        result_type result(dont_care) const
        {
            return this->samples;
        }
    private:
        // TODO this should be seeded with something
        boost::mt19937 gen;
        int max_samples;
        std::vector<Sample> samples;
    };

}

namespace tag {
    struct sampler 
        : depends_on<count>,
          sampler_sample_size {
        typedef typename accumulators::impl::sampler_impl<mpl::_1> impl;
    };
}

namespace extract {
    extractor<tag::sampler> const sampler = {};

    BOOST_ACCUMULATORS_IGNORE_GLOBAL(sampler)
}

using extract::sampler;


}}

