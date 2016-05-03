#include <stdlib.h>

#include "gtest/gtest.h"

#include <stdlib.h>
#include <iostream>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include "topk.hpp"
#include <boost/foreach.hpp>

using namespace boost::accumulators;

void test_topk() {
    std::cout << "topk test" << std::endl;
    accumulator_set<int, stats<tag::topk> > acc;

    for( int i = 0; i < 100; ++i ) {
        for( int k = 0; k < i; ++k ) {
            acc(i);
        }
    }

    auto topk_values = topk(acc);
    for (auto&& i : topk_values) std::cout << i << std::endl;
    std::cout << "\n";
}

#if 0
TEST (UniqueTest, Integers) { 
    std::cout << "unique test" << std::endl;
    accumulator_set<int, stats<tag::unique> > acc;

    for( int i = 0; i < 10; ++i ) {
        for( int k = 0; k < i; ++k ) {
            acc(i);
        }
    }

    auto unique_count = unique(acc);
    for( int i = 0; i < 10; ++i ) {
        EXPECT_EQ(unique_count[i], i);
        std::cout << "map[" << i << "] = " << unique_count[i] << std::endl;
    }
}
#endif

int main(int argc, char **argv) {
    test_topk();
    /*
      ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    */
}


