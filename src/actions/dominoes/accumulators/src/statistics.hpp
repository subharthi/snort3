#ifndef __STATISTICS_HPP__
#define __STATISTICS_HPP__

// TODO go through the headers and determine which are still necessary
#include "accumulator_group.hpp"
#include "packet_extractor.hpp"

#include "types.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <utility>

#include <boost/parameter/name.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/parameter/preprocessor.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>

////////////////////////////////////////////////////////////////////////////////
// accumulator_table
class accumulator_table {
    key_value_template_type key_value_template;
    // TODO get the packet extractors working correctly
    //packet_extractor<std::string> key_extractor;
    //packet_extractor<std::vector<int> > value_extractor;
    metric_type_list type_list;
public:
    accumulator_map_type data;

    accumulator_table(key_value_template_type key_value_temp, metric_type_list const &type) :
        key_value_template(key_value_temp),
        //key_extractor(key_value_temp.first),
        //value_extractor(key_value_temp.second),
        type_list(type) {
        //packet_extractor<std::string> key_extractor(key_value_temp.first);
        //packet_extractor<std::vector<int> > value_extractor(key_value_temp.second);
    }

    void operator()(Packet const &packet);
    accumulator_group<uint32_t>& operator[](std::string const &key);

    void operator+=(accumulator_table &other);

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(type_list);
        ar & BOOST_SERIALIZATION_NVP(data);
    }

    // Debugging function
    friend std::ostream& operator<<(std::ostream& out, const accumulator_table& table);
}; /* class accumulator_table */

////////////////////////////////////////////////////////////////////////////////
// Statistics
class Statistics {
    boost::mutex mtx_;
    accumulator_table_map_type data;

public:
    Statistics() {};

    void operator()(Packet const &packet);
    // Throws an exception if the key does not exist
    accumulator_table& operator[](std::string const &name);
    void add(std::string const &name, const var_template_type &key, const var_template_type &value, const metric_type_list &type_list);
    // Return a copy of the map of accumulator tables and then clear it
    accumulator_table_map_type dump();

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(data);
    }

    // Debugging function
    friend std::ostream& operator<<(std::ostream& out, const Statistics& stats);
}; /* class Statistics */

#endif // __STATISTICS_HPP__

