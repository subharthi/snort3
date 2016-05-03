#include "statistics.hpp"
#include "extractors/basic.hpp"

#include <stdlib.h>
#include <iostream>

#include "gtest/gtest.h"

#include <fstream>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

TEST (BlankTest, one) { 
    Statistics statistics;
    statistics.add("", {},{}, {});
    Packet fake_pkt1( 1 );
    statistics(fake_pkt1);
    std::cerr << statistics << std::endl;
    EXPECT_EQ(1, 1);
}

TEST (BlankTest, two) { 
    Statistics statistics;
    statistics.add("", {""},{""}, {});
    Packet fake_pkt1( 1 );
    statistics(fake_pkt1);
    std::cerr << statistics << std::endl;
    EXPECT_EQ(1, 1);
}

TEST (BlankTest, three) { 
    Statistics statistics;
    statistics.add("", {},{}, {metric::count});
    Packet fake_pkt1( 1 );
    statistics(fake_pkt1);
    std::cerr << statistics << std::endl;
    EXPECT_EQ(1, 1);
}

TEST (BlankTest, four) { 
    Statistics statistics;
    statistics.add("", {""},{""}, {metric::count});
    Packet fake_pkt1( 1 );
    statistics(fake_pkt1);
    std::cerr << statistics << std::endl;
    EXPECT_EQ(1, 1);
}

TEST (BlankTest, five) { 
    Statistics statistics;
    statistics.add("", {},{}, {metric::count});
    Packet fake_pkt1( 1 );
    statistics(fake_pkt1);
    std::cerr << statistics << std::endl;
    EXPECT_EQ(1, 1);
}

//#define METRIC(NAME) \
//TEST(SanityTest, NAME) { \
//    using namespace extractors; \
//    Statistics statistics; \
//    statistics.add("", {packet::src_ip}, {packet::src_ip}, {metric::NAME}); \
//    Packet fake_pkt1( 1 ); \
//    statistics(fake_pkt1); \
//    std::cerr << statistics << std::endl; \
//    EXPECT_EQ(1, 1); \
//}
//#define METRIC_(NAME, PARAM) \
//TEST(SanityTest, NAME ## _ ## PARAM) { \
//    using namespace extractors; \
//    Statistics statistics; \
//    statistics.add("", {packet::src_ip}, {packet::src_ip}, {metric::NAME ## _ ## PARAM}); \
//    Packet fake_pkt1( 1 ); \
//    statistics(fake_pkt1); \
//    std::cerr << statistics << std::endl; \
//    EXPECT_EQ(1, 1); \
//}
//#include "metrics.hpp"
//#undef METRIC
//#undef METRIC_

TEST (CountTest, small) { 
    using namespace extractors;
    Statistics statistics;

    statistics.add("", {packet::src_ip}, {packet::src_ip}, {metric::count});

    Packet fake_pkt1( 1 );

    statistics(fake_pkt1);

    EXPECT_EQ(extract_result<tag::count>(statistics[""]["1"]), 1);
}

TEST (CountTest, large) { 
    using namespace extractors;
    Statistics statistics;
    int count = 100;

    statistics.add("", {packet::src_ip}, {packet::src_ip}, {metric::count});

    Packet fake_pkt1( 1 );

    for( int i = 0; i < count; ++i ) {
        statistics(fake_pkt1);
    }

    EXPECT_EQ(extract_result<tag::count>(statistics[""]["1"]), count);
}

TEST (Serialization, simple) { 
    using namespace extractors;
    Statistics statistics;
    int count = 100;

    statistics.add("", {packet::src_ip}, {packet::src_ip}, {metric::count});

    Packet fake_pkt1( 1 );

    for( int i = 0; i < count; ++i ) {
        statistics(fake_pkt1);
    }

    Packet fake_pkt2( 2 );

    for( int i = 0; i < count; ++i ) {
        statistics(fake_pkt2);
    }

    Packet fake_pkt3( 3 );

    for( int i = 0; i < count; ++i ) {
        statistics(fake_pkt3);
    }

    std::cout << std::endl;
    boost::archive::xml_oarchive xml_archive(std::cout);
    xml_archive << BOOST_SERIALIZATION_NVP(statistics);

    EXPECT_EQ(extract_result<tag::count>(statistics[""]["1"]), count);
}

/*
TEST (FullTest, large) { 
    using namespace extractors;
    Statistics statistics;
    int count = 100;

    statistics.add("", {packet_data::src_ip}, {packet_data::dst_ip}, 
                   {metric::count, metric::mean, metric::min, metric::max, metric::sum, 
                    metric::kurtosis, metric::median, metric::skewness, metric::lazy_variance,
                    metric::moment_1, metric::moment_2, metric::moment_3, metric::moment_4});

    Packet fake_pkt1( 1 );
    for( int i = 0; i < count; ++i ) {
        statistics(fake_pkt1);
    }

    EXPECT_EQ(extract_result<tag::count>(statistics[""]["1"]), count);
}
*/

int main(int argc, char **argv) {
      ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}


