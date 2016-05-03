#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <map>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include "../new_accumulators/entropy.hpp"

using namespace boost::accumulators;
std::vector<int> vec = { 0, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 1, 900, 900, 899, 877, 532, 234, 234, 12, 13, 15, 16, 16, 17, 29, 30, 45, 235, 2, 4, 5, 3, 2 };

double shannon_entropy(std::vector<int> vec) {
    double entropy = 0;
    double max_entropy = 0;

    std::map<int, int>frequency;
    for( auto &iter : vec ) {
        frequency[iter]++;
    }

    for( auto &iter : frequency ) {
        double prob = ((double) iter.second) / vec.size();
        entropy += prob * log2(prob);
    }
    entropy = entropy * -1;

    max_entropy = log2(frequency.size());

    std::cout << "Entropy: " << entropy << std::endl;
    std::cout << "Max Entropy: " << max_entropy << std::endl;
    std::cout << max_entropy / entropy << std::endl;

    return entropy;
}

int main() {
    std::cout << "Calculating Entropy" << std::endl;

    std::cout << "Entropy: " << shannon_entropy( vec ) << std::endl;
    std::cout << "----------------------------------" << std::endl;

    accumulator_set<int, stats<tag::entropy> > acc( sampler_sample_size = 10 );

    for( auto& i : vec ) {
        acc(i);
    }
 
    std::cout << "Accumulator entropy: " << entropy(acc) << std::endl;
}

