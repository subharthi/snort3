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


#include "../../src/accumulator_group.hpp"

using namespace boost::accumulators;

int main() {
    typedef features< 
    droppable<tag::count>,
    droppable<tag::sum> > features;

    typedef accumulator_set<int, features > set_type;

    accumulator_group<int>  group{ {metric::count, metric::sum} };

    set_type set1(tag::rolling_window::window_size = 5);

    for( int i = 0; i < 10; ++i ) { 

        set_type set2(tag::rolling_window::window_size = 5);

        set2(i);
        set1(i);
        group += set2;

    }

}

