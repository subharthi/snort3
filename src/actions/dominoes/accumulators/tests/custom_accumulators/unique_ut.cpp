#include "gtest/gtest.h"

#include <stdlib.h>
#include <iostream>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include "unique.hpp"
#include <boost/foreach.hpp>

// Removed while the unique accumulator is being edited
#if 0
using namespace boost::accumulators;

void test_unique() {
    std::cout << "unique test" << std::endl;
    accumulator_set<int, stats<tag::unique> > acc;

    for( int i = 0; i < 10; ++i ) {
        for( int k = 0; k < i; ++k ) {
            acc(i);
        }
    }
 
    for (auto&& i : unique(acc)) 
        std::cout << "map[" << i.first << "] = " << i.second << std::endl;
    std::cout << "\n";
}

TEST (UniqueTest, Empty) { 
    accumulator_set<int, stats<tag::unique> > acc;

    unique(acc);
}

TEST (UniqueTest, Integers) { 
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

int main(int argc, char **argv) {
      ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}
#endif

