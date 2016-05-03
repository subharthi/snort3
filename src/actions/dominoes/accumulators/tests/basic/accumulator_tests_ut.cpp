#include "boost/accumulators/accumulators.hpp"
#include <boost/accumulators/statistics.hpp>
#include <stdlib.h>
#include <iostream>

#include "gtest/gtest.h"

using namespace boost::accumulators;

TEST (CountAccumulator, none) { 
    accumulator_set<int, stats<tag::count> > set;
    EXPECT_EQ(extract_result<tag::count>(set), 0);
}

TEST (CountAccumulator, one) { 
    accumulator_set<int, stats<tag::count> > set;
    set(0);
    EXPECT_EQ(extract_result<tag::count>(set), 1);
}
/*
 * Read the test data from a file
 */
/*
TEST(test, test) {
    accumulator_set<int, stats<feature> > set;

    open("data")
    while(in.good()) {
        in >> number;
        set(number);
    }

    size_t result = extract_result<feature> >(set);
    open("results")
    expected = get_value(feature);

    std::string description = open("description");
    EXPECT_EQ(result, expected) << description
}
*/

int main(int argc, char **argv) {
      ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

