#include "statistics.hpp"
#include "extractors/basic.hpp"
#include <stdlib.h>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>



void print_table(accumulator_table_map_type &table) {
    for( auto &i : table ) {
        for( auto &k : i.second.data ) {
            std::cout << "\"" << i.first << "\": ";
            std::cout << "  \"" << k.first << "\":  ";
            //std::cout << k.second;
            std::cout << extract_result<tag::count>(k.second) << std::endl;
        }
    }
}

struct accumulate {
    accumulate(Statistics &s, int c, int m) : statistics(s), count(c), timer(m) { }
    // TODO record the number of times something was input in thread local storage
    // then make sure at the end of the test all the counts in thread local storage combined add up to the count in the accumulator
    void operator()() {
        for(int i = 0; i < count; ++i ) {
            Packet fake_pkt1( i%5 );
            statistics(fake_pkt1);
            boost::this_thread::sleep(timer);
        }
    }

private:
    boost::posix_time::milliseconds timer;
    int count;
    Statistics &statistics;
};

struct dump {
    dump(Statistics &s, int c, int m) : statistics(s), count(c), timer(m) { }
    void operator()() {
        uint64_t total = 0;
        for(int i = 0; i < count; ++i ) {
                boost::this_thread::sleep(timer);
                accumulator_table_map_type table = statistics.dump();
                print_table(table);
        }
        //std::cout << "Total: " << total << std::endl;
    }

private:
    boost::posix_time::milliseconds timer;
    int count;
    Statistics &statistics;
};

int main(int argc, char *argv[]) {

    int num_inputters = 1;

    if( argc <= 1 ) {
        num_inputters = 1;
    } else {
        num_inputters = atoi(argv[1]);
    }

    using namespace extractors;
    Statistics statistics;
    statistics.add("test", {packet::src_ip}, {packet::src_ip}, {metric::count});
    statistics.add("test2", {packet::src_ip}, {packet::src_ip}, {metric::count});

    boost::thread **inputters = new boost::thread*[num_inputters];
    for( int i = 0; i < num_inputters; ++i ) {
        inputters[i] = new boost::thread{accumulate(statistics, 10000, 1)};
    }
    boost::thread outputter{dump(statistics, 200, 100)};
    boost::thread outputter2{dump(statistics, 200, 100)};

    for( int i = 0; i < num_inputters; ++i ) {
        inputters[i]->join();
        std::cout << "joined inputter " << i << std::endl;
    }
    outputter.join();
    std::cout << "joined outputer 1" << std::endl;
    outputter2.join();
    std::cout << "joined outputer 2" << std::endl;

    accumulator_table_map_type table = statistics.dump();

    print_table(table);

}
