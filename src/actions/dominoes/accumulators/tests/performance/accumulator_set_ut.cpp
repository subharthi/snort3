/*
 * =====================================================================================
 *
 *       Filename:  accumulator_set_ut.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/18/15 17:56:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <iostream>
#include <climits>
#include <time.h>

#include <vector>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

using namespace boost::accumulators;



template <class tag>
struct set {
    set(int n) : num(n) {}

    void operator()() {
        //boost::random::uniform_int_distribution<> rand(0, 100000);
        accumulator_set<int, stats<tag> > acc_set;

        for(unsigned long i = 0; i < num; ++i) {
            //acc_set(rand(gen));
            acc_set(1);
        }

        std::cout << extract_result<tag>(acc_set) << std::endl;
    }
private:
    int num;
    boost::mt19937 gen;
};

template<class test_function>
int benchmark(test_function test) {
    clock_t num_cycles;
    num_cycles = clock();
    test();
    num_cycles = clock() - num_cycles;
    return num_cycles;
}

int main(int argc, char *argv[]) {
    //test_no_data();
    if(argc<2) {
        std::cout << "Too few arguments" << std::endl;
        exit(0);
    }
    int num = atoi(argv[1]);
    int cycles = 0;

#define METRIC(NAME) \
    cycles = benchmark(set<tag::NAME>(num)); \
    std::cout << #NAME " cycles: " << cycles << std::endl;
#define METRIC_(NAME, PARAM) \
    cycles = benchmark(set<tag::NAME<PARAM> >(num)); \
    std::cout << #NAME "_" #PARAM " cycles: " << cycles << std::endl;
#include "metrics.hpp"
#undef METRIC
#undef METRIC_

}

/*

TODO test multiple accumulators
TODO determine the cost for extracting results and accumulating differently

 */
