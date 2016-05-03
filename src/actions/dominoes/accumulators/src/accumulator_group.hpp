
#ifndef __ACCUMULATOR_GROUP_HPP__
#define __ACCUMULATOR_GROUP_HPP__

// TODO go through the headers and determine which are still necessary
#include <stdlib.h>
#include <iostream>

#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>

#include "../accumulators_extension.hpp"
#include <string>
#include <sstream>
#include "statistics_inc.hpp"

#include "enhanced_set.hpp"
#include "metrics.hpp"

#include <boost/mpl/vector.hpp>
#include <boost/lexical_cast.hpp>
#include <map>

#include <boost/mpl/for_each.hpp>

#include <algorithm>

#include <boost/blank.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/at_key.hpp>

#include <boost/mpl/map.hpp>
#include <boost/mpl/at.hpp>
// figure out the issue with the accumulator's parameters

namespace metric {
    enum metric_type {
        BOOST_PP_SEQ_ENUM(METRICS_SEQ_1),
        METRIC_TYPE_LAST,
    };

    // TODO don't define variables in headers
    //extern const std::map<metric_type, std::string> metric_type_decoder_;
    // map the metric_type enum to each values string
    static const std::map<metric_type, std::string> metric_type_decoder_ = {
        METRIC_TYPE_DECODER
    };

    static inline const std::string metric_type_decoder(metric_type value) {
        if(value < metric::METRIC_TYPE_LAST) {
            return metric_type_decoder_.at(value);
        }
        return boost::lexical_cast<std::string>(value);
    }
}

typedef std::vector<metric::metric_type> metric_type_list;

// This is a list of the metric types to allow
typedef boost::mpl::vector<
BOOST_PP_SEQ_ENUM(METRICS_SEQ_2)
>::type feature_mpl_vector;

// This is used to map feature tags to their enum value
typedef boost::mpl::map<
TAG_METRIC_SEQ
> tag_to_metric_map_type;


typedef typename boost::mpl::transform<feature_mpl_vector, droppable<boost::mpl::_1> >::type droppable_feature_mpl_vector;
typedef typename boost::mpl::transform<feature_mpl_vector, boost::add_pointer<boost::mpl::_1> >::type pointers_feature_mpl_vector;

// TODO default parameters will be needed to prevent compilation errors
//typedef
//    parameter::parameters<
//        parameter::required<tag::accumulator>
//      , parameter::optional<tag::sample>
//      // ... and others which are not specified here...
//    >
//accumulator_params;

//template <metric::metric_type T>
//struct to_feature {
//    typedef typename boost::mpl::at<feature_mpl_vector,boost::mpl::int_<static_cast<int>(T)> >::type type;
//};

////////////////////////////////////////////////////////////////////////////////
// class accumulator_group
// An accumulator_group is a enhanced_set with all possible accumulators that
// drops any accumulator not specified in the constructor
//
template <typename Sample>
class accumulator_group : 
    public enhanced_set<Sample, droppable_feature_mpl_vector, void> {

        struct dropping_visitor {
            dropping_visitor(accumulator_group &acc_group_) : acc_group(acc_group_) { }

            template <typename T>
            void operator()(T*) const {
                metric::metric_type m = static_cast<metric::metric_type>(boost::mpl::at<tag_to_metric_map_type, T>::type::value);
                if(!(std::find(acc_group.metrics_list.begin(), acc_group.metrics_list.end(), m) 
                            != acc_group.metrics_list.end())) {
                    acc_group.template drop<T>();
                }
            }
        private:
            accumulator_group &acc_group;
        };
public:
    const metric_type_list &metrics_list;

    accumulator_group(const metric_type_list &metrics_list_) : metrics_list(metrics_list_) {
        boost::mpl::for_each<pointers_feature_mpl_vector>(dropping_visitor(*this));
    }

}; /* class accumulator_group */

#endif //__ACCUMULATOR_GROUP_HPP__
