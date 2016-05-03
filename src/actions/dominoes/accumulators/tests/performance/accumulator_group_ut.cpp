#include "accumulator_group.hpp"
#include <stdlib.h>
#include <iostream>
#include <climits>
#include <time.h>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>


void test_no_data() {
    accumulator_group group( { metric::count } );

    std::cout << group << std::endl;
}


struct count_group {
    count_group(int n) : num(n) {}
    void operator()() {
        boost::random::uniform_int_distribution<> rand(0, 100000);
        accumulator_group group( { metric::count } );
        for(unsigned long i = 0; i < num; ++i) {
            group(rand(gen));
        }
        std::cout << group << std::endl;
        // TODO these will be supported
        //std::cout << count(group) << std::endl;
        //std::cout << extract_result<tag::count>(group) << std::endl;
        //std::cout << group.count() << std::endl;
        //std::cout << group.extract_result<tag::count>() << std::endl;

        // TODO this will be the important one
        // the others just make an accumulator_group behave like an accumulator_set
        // this makes it possible to choose what is being extracted at runtime
        std::cout << group.extract_result<tag::count>() << std::endl;
        try {
            std::cout << group.extract_result<tag::mean>() << std::endl;
        } catch ( const char * e ) {
            std::cout << e << std::endl;
        }
    }
private:
    int num;
    boost::mt19937 gen;
};

struct printer {
    printer() {}
    template <typename accumulator>
    void operator()(accumulator a) {
        std::cout << "VISITED" << std::endl;
    }
};

struct count_set {
    count_set(int n) : num(n) {}

    void operator()() {
        boost::random::uniform_int_distribution<> rand(0, 100000);
        accumulator_set<value_type, stats<tag::count> > set;

        for(unsigned long i = 0; i < num; ++i) {
            set(rand(gen));
        }

        std::cout << extract_result<tag::count>(set) << std::endl;
    }
private:
    int num;
    boost::mt19937 gen;
};

struct all_group {
    all_group(int n) : num(n) {}
    void operator()() {
        boost::random::uniform_int_distribution<> rand(0, 100000);
        accumulator_group group( { 
                metric::mean,
                metric::min,
                metric::max,
                metric::sum,
                metric::count,
                metric::kurtosis,
                metric::skewness,
                metric::lazy_variance,
                },
                tag::rolling_window::window_size = 2
                );
//#define METRIC(NAME) metric::NAME
//#define METRIC_(NAME, PARAM) metric::NAME ## _ ## PARAM
//#include "metrics.hpp"
//#undef METRIC
//#undef METRIC_
        for(unsigned long i = 0; i < num; ++i) {
            group(rand(gen));
        }
        std::cout << group << std::endl;
        //std::cout << group.extract_result<tag::mean> << std::endl;
    }
private:
    int num;
    boost::mt19937 gen;
};

struct all_set {
    all_set(int n) : num(n) {}

    void operator()() {
        boost::random::uniform_int_distribution<> rand(0, 100000);
        accumulator_set<value_type, stats<
            tag::mean,
            tag::min,
            tag::max,
            tag::sum,
            tag::count,
            tag::kurtosis,
            tag::skewness,
            tag::lazy_variance,
            tag::moment<1>,
            tag::moment<2>,
            tag::moment<3>,
            tag::moment<4>,
            tag::rolling_sum
            > > set(tag::rolling_window::window_size = 5);

        for(unsigned long i = 0; i < num; ++i) {
            set(rand(gen));
        }

        std::cout << 
            extract_result<tag::mean>(set) << ", " <<
            extract_result<tag::min>(set) << ", " <<
            extract_result<tag::max>(set) << ", " <<
            extract_result<tag::sum>(set) << ", " <<
            extract_result<tag::count>(set) << ", " <<
            extract_result<tag::rolling_sum>(set) << ", "
            << std::endl;
    }
private:
    int num;
    boost::mt19937 gen;
};

struct random_calc {
    random_calc(int n) : num(n) {}

    void operator()() {
        boost::random::uniform_int_distribution<> rand(0, 100000);
        for(unsigned long i = 0; i < num; ++i) {
            rand(gen);
        }
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
    int num;
    if(argc<2) {
        num = 1;
    } else {
        num = atoi(argv[1]);
    }

    int group_cycles = 0;
    int set_cycles = 0;
    int random_cycles = 0;

    random_cycles = benchmark(random_calc(num));
    std::cout << "random_cycles: " << random_cycles << std::endl;
    std::cout << "group" << std::endl;
    group_cycles = benchmark(count_group(num))-random_cycles;
    std::cout << "Cycles: " << group_cycles << std::endl;
    std::cout << "set" << std::endl;
    set_cycles = benchmark(count_set(num))-random_cycles;
    std::cout << "Cycles: " << set_cycles << std::endl;
    std::cout << "Ratio: " <<  ((double) set_cycles)/group_cycles << std::endl;

    std::cout << "Test #2" << std::endl;
    group_cycles = benchmark(all_group(num))-random_cycles;
    std::cout << "Cycles: " << group_cycles << std::endl;
    set_cycles = benchmark(all_set(num))-random_cycles;
    std::cout << "Cycles: " << set_cycles << std::endl;
    std::cout << "Ratio: " <<  ((double) set_cycles)/group_cycles << std::endl;


}

