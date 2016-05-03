#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include "fwd_decls.hpp"

#include <string>
#include <map>
#include <vector>
#include <boost/variant.hpp>
#include <boost/ref.hpp>

#include <cinttypes>



// the type of the key used to access accumulator groups in the map
typedef std::string key_type;
// the type of the value to be input into the accumulator group
typedef int value_type;
// the pair of the key and value types
typedef std::pair<key_type, value_type> key_value_type;
// the map that the key and value are insert into
typedef std::map<key_type, accumulator_group<uint32_t> > accumulator_map_type;

// the type of the variable for the template used to turn packet data into the key/value pair
typedef std::vector<boost::variant<std::string, packet_data_container> > var_template_type;
//typedef std::vector<boost::variant<std::function<Packet()> > var_template_type;
// the pair of key and value templates
typedef const std::pair<var_template_type, var_template_type> key_value_template_type;
// A map of the names of accumulator tables and the accumulator tables themselves
typedef std::map<std::string, accumulator_table> accumulator_table_map_type;

#endif

