#include <stdlib.h>
#include <iostream>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/accumulators/accumulators.hpp>

#include <boost/accumulators/statistics/stats.hpp>

#include "accumulators_extension.hpp"
#include <string>
#include <sstream>

#include "statistics/rolling_window.hpp"
#include "statistics/rolling_sum.hpp"
#include "statistics/rolling_count.hpp"
#include "statistics/rolling_mean.hpp"
#include "statistics/count.hpp"
#include "statistics/p_square_quantile.hpp"
#include "statistics/moment.hpp"
#include "statistics/min.hpp"
#include "statistics/max.hpp"
#include "statistics/sum.hpp"
#include "statistics/kurtosis.hpp"
#include "statistics/median.hpp"
#include "statistics/mean.hpp"
#include "statistics/skewness.hpp"
#include "statistics/variance.hpp"
#include "statistics/rolling_moment.hpp"
#include "statistics/density.hpp"
#include "statistics/sum_kahan.hpp"
#include "statistics/tail.hpp"
#include "statistics/pot_quantile.hpp"

#include <fstream>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "../../src/enhanced_set.hpp"


using namespace boost::accumulators;

int main() {

    //typedef features< 
    //    tag::count,
    //    droppable<tag::min>,
    //    droppable<tag::max>,
    //    droppable<tag::median>,
    //    droppable<tag::rolling_mean>,
    //    droppable<tag::rolling_sum>,
    //    droppable<tag::sum_kahan>,
    //    droppable<tag::variance> > features;
    typedef features< 
        droppable<tag::min>,
        droppable<tag::max>,
        droppable<tag::mean>,
        droppable<tag::median>,
        droppable<tag::rolling_mean>,
        droppable<tag::sum_kahan>,
        droppable<tag::variance>,
        droppable<tag::moment<2> >,
        droppable<tag::moment<3> >

        > features;

    // TODO fix the moment issue
    typedef accumulator_set<int, features > set_type;
    typedef accumulator_set<int, stats<droppable<tag::min>, droppable<tag::max> > > droppable_set_type;

    droppable_set_type droppable;
    droppable(1);
    droppable.drop<tag::min>();
    std::cout << "droppable " << min(droppable) << std::endl;
    //{tag::rolling_window::window_size = 5};

    enhanced_set<int, features, void>  m{tag::rolling_window::window_size = 5, tag::density::cache_size = 5, tag::density::num_bins = 1};
    //enhanced_set<int, stats<tag::min, tag::max>, void >  m{tag::rolling_window::window_size = 5};

    set_type set1(tag::rolling_window::window_size = 5);

    //std::cout << "name: " << tag::count::name() << std::endl;
    //std::cout << "name: " << tag::min::name() << std::endl;
    //std::cout << "name: " << tag::max::name() << std::endl;
    //std::cout << "name: " << tag::skewness::name() << std::endl;
    //std::cout << "name: " << tag::median::name() << std::endl;
    //std::cout << "name: " << tag::moment<1>::name() << std::endl;
    //std::cout << "name: " << tag::moment<2>::name() << std::endl;
    //std::cout << "name: " << tag::moment<3>::name() << std::endl;
    //std::cout << "name: " << tag::moment<4>::name() << std::endl;
    //std::cout << "name: " << tag::moment<5>::name() << std::endl;
    //std::cout << "name: " << tag::rolling_sum::name() << std::endl;
    //std::cout << "name: " << tag::rolling_count::name() << std::endl;
    //std::cout << "name: " << tag::rolling_mean::name() << std::endl;
    //std::cout << "name: " << tag::rolling_window::name() << std::endl;
    //std::cout << "name: " << tag::kurtosis::name() << std::endl;
    //std::cout << "name: " << tag::density::name() << std::endl;
    //std::cout << "name: " << tag::mean::name() << std::endl;
    //std::cout << "name: " << tag::immediate_mean::name() << std::endl;
    //std::cout << "name: " << tag::immediate_mean_of_weights::name() << std::endl;
    //std::cout << "name: " << tag::rolling_mean::name() << std::endl;
    //std::cout << "name: " << tag::mean_of_weights::name() << std::endl;

    m.drop<tag::min>();
    
    for( int i = 0; i < 10; ++i ) { 

        set_type set2(tag::rolling_window::window_size = 5);

        set2(i);
        set1(i);
        m += set2;
        
        //std::cout << "mean:  " << extract_result<tag::mean>(m) << std::endl;
        //std::cout << "rolling_sum:  " << extract_result<tag::rolling_sum>(m) << std::endl;

        //std::cout << "mean:  " << extract_result<tag::mean>(set1) << std::endl;
        //std::cout << "rolling_sum:  " << extract_result<tag::rolling_sum>(set1) << std::endl;
        //std::cout << std::endl;
        //std::cout << "min:  " << extract_result<tag::min>(m) << std::endl;
        //std::cout << "max:  " << extract_result<tag::max>(m) << std::endl;
        //std::cout << "kurtosis: " << extract_result<tag::kurtosis>(m) << std::endl;
        //std::cout << "median:   " << extract_result<tag::median>(m) << std::endl;
        //std::cout << "skewness: " << extract_result<tag::skewness>(m) << std::endl;
        //std::cout << "count:    " << extract_result<tag::count>(m) << std::endl;
        //std::cout << "sum:  " << extract_result<tag::sum>(m) << std::endl;
        //std::cout << "lazy_variance:    " << extract_result<tag::lazy_variance>(m) << std::endl;
        //std::cout << "mean: " << extract_result<tag::mean>(m) << std::endl;
        ////std::cout << "moment 1" << extract_result<tag::moment<1> >(m) << std::endl;
        //std::cout << "moment 2" << extract_result<tag::moment<2> >(m) << std::endl;
        //std::cout << "moment 3" << extract_result<tag::moment<3> >(m) << std::endl;
        //std::cout << "moment 4" << extract_result<tag::moment<4> >(m) << std::endl;

        //std::cout << "set results" << std::endl;
        //std::cout << "moment 2" << extract_result<tag::moment<2> >(set1) << std::endl;
        //std::cout << "moment 3" << extract_result<tag::moment<3> >(set1) << std::endl;
        //std::cout << "moment 4" << extract_result<tag::moment<4> >(set1) << std::endl;
    }

    // Save the enhanced_set
    std::cout << std::endl;
    boost::archive::xml_oarchive xml_archive(std::cout);
    xml_archive << BOOST_SERIALIZATION_NVP(m);

    std::ofstream ofs("some_file");
    boost::archive::xml_oarchive xml_archive2(ofs);
    xml_archive2 << BOOST_SERIALIZATION_NVP(m);

    // Load the enhanced_set
    //enhanced_set<int, features, void>  serialize_test{tag::rolling_window::window_size = 5, tag::density::cache_size = 5, tag::density::num_bins = 1};

    //std::ifstream ifs("some_file");
    //boost::archive::xml_iarchive ia(ifs);
    //ia >> serialize_test;

    //min(serialize_test);
    //max(serialize_test);
    //count(serialize_test);
    //sum(serialize_test);
    //std::cout << "results: " << extract_result<tag::count>(set1) << std::endl;

    //accumulator_set accumulator_set_2<int, stats<count> >();

    //accumulator_set_1.visit_if<is_mergable>(enhanced_set(accumulator_set_2));
}

