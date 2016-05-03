#include "gtest/gtest.h"

#include <stdlib.h>
#include <iostream>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include "sampler.hpp"
#include "rolling_sampler.hpp"
#include <boost/foreach.hpp>

using namespace boost::accumulators;

void test_sampler() {
    std::cout << "sampler test" << std::endl;
    accumulator_set<int, stats<tag::sampler> > acc( sampler_sample_size = 10 );

    for( int i = 0; i < 100; ++i ) {
        acc(i);
    }
 
    for (auto&& i : sampler(acc)) std::cout << i << ' ';
    std::cout << "\n";
}

void test_rolling_sampler() {
    std::cout << "rolling sampler test" << std::endl;
    accumulator_set<int, stats<tag::rolling_sampler> > acc( rolling_sampler_sample_size = 10 );

    for( int i = 0; i < 100; ++i ) {
        acc(i);
    }
 
    //for (auto&& i : rolling_sampler(acc)) std::cout << i << ' ';
    BOOST_FOREACH( int i, rolling_sampler(acc) ) {
        std::cout << i << ' ';
    }
    std::cout << "\n";
}

#if 0
void test_weighted_sampler() {
    accumulator_set<int, stats<tag::weighted_sampler> > acc( sampler_sample_size = 20 );

    for( int i = 0; i < 100; ++i ) {
        acc(i);
    }
 
    for (auto&& i : weighted_sampler(acc)) std::cout << i << ' ';
    std::cout << "\n";
}
#endif



int main(int argc, char **argv) {

    test_sampler();
    test_rolling_sampler();
    //test_weighted_sampler();

}
