
// TODO go through the headers and determine which are still necessary
#include "statistics.hpp"

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// class accumulator_table
void accumulator_table::operator()(Packet const &packet) {
    // turn the key value template into an actual key value pair
    // TODO packet_extractors return vectors but right now the key and value types can't handle that
    // TODO this shouldn't be created every time
    packet_extractor<std::string> key_extractor(key_value_template.first);
    packet_extractor<std::vector<uint64_t> > value_extractor(key_value_template.second);
    
    key_type key     = key_extractor(packet);
    value_type value = value_extractor(packet).front();
     
    // If it's not in the data map create it first otherwise just find it
    auto iter_pair = data.emplace(key, type_list);
    iter_pair.first->second(value);
}

accumulator_group<uint32_t>& accumulator_table::operator[](std::string const &key) {
    return data.at(key);
}

void accumulator_table::operator+=(accumulator_table &other) {
    for( auto &i : other.data ) {
        auto iter = data.find(i.first);
        if( iter == data.end() ) {
            data.insert(i);
            continue;
        } else {
            iter->second += i.second;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// class Statistics
void Statistics::operator()(Packet const &packet) {
    boost::lock_guard<boost::mutex> guard(mtx_);
    // iterator over each accumulator table and record values from the packet
    for(auto &iter : data) {
        iter.second(packet);
    }
}

// Throws an exception if the key does not exist
accumulator_table& Statistics::operator[](std::string const &name) {
    boost::lock_guard<boost::mutex> guard(mtx_);
    return data.at(name);
}

void Statistics::add(std::string const &name, const var_template_type &key, const var_template_type &value, const metric_type_list &type_list) {
    boost::lock_guard<boost::mutex> guard(mtx_);
    // TODO consider using emplace here
    data.insert(
            std::pair<std::string,accumulator_table>(
                name, accumulator_table(key_value_template_type(key, value), type_list)));
}

// Return a copy of the map of accumulator tables and then clear it
accumulator_table_map_type Statistics::dump() {
    boost::lock_guard<boost::mutex> guard(mtx_);
    accumulator_table_map_type ret = data;
    for( auto &i : data ) {
        i.second.data.clear();
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
// Debugging functions
class printer : public boost::static_visitor<> {
public:
    printer(std::ostream &o) : out(o) { }

    template <typename T>
    void operator()(const T& t) const {
        out << t;
    }

    void operator()(const std::string& t) const {
        out << "\"" << t << "\"";
    }

private:
    std::ostream &out;
};

std::ostream& operator<<(std::ostream& out, const accumulator_table& table) {
    //out << "key_template={";
    //for( const auto &i : table.key_value_template.first ) {
    //    boost::apply_visitor(printer(out), i);
    //    out << ", ";
    //}
    //out << "}" << std::endl;
    //out << "value_template={";
    //for( const auto &i : table.key_value_template.second ) {
	//	boost::apply_visitor(printer(out), i);
    //    out << ", ";
    //}
    //out << "}" << std::endl;

    //out << "type_list={";
    //for(metric::metric_type const &iter : table.type_list) {
    //    out << iter << ", ";
    //}
    //out << "}" << std::endl;

    //for(const auto& i : table.data) {
    //    std::cout << "key=\"" << i.first << "\"" << std::endl;
    //    std::cout << i.second << std::endl;
    //}
    return out;
}

// WARNING THIS IS ONLY FOR DEBUGGING IT IS NOT THREAD SAFE
std::ostream& operator<<(std::ostream& out, const Statistics& stats) {
    //out << "statistics={" << std::endl;
    //for( auto const &i : stats.data ) {
    //    out << "\"" << i.first << "\"" << std::endl;
    //    out << i.second << std::endl;
    //}
    //out << "}";
    return out;
}

