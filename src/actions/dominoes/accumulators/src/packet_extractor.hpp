#ifndef __PACKET_HPP__
#define __PACKET_HPP__

#include "types.hpp"
#include <string>
#include <inttypes.h>

struct Inspector;
// TODO this should probably not be included and just make everything templates
#ifndef REAL_PACKETS
#include "include/packet.hpp"
#else
#include "protocols/packet.h"
#endif

#include <exception>

typedef std::map<uint64_t,uint64_t> ip_usr_map_type;

class value_not_ready: public std::exception {
public:        
	value_not_ready(std::string _msg): msg(_msg) {}
	virtual const char* what() const throw(){
		return msg.c_str();
	}
private:
	std::string msg;
};

struct packet_data_args {
   packet_data_args() {}
   virtual uint64_t operator()(const Packet &value) const  = 0;
   virtual std::ostream& print(std::ostream& out) const = 0;
   virtual std::string stringify() const = 0 ;	     	
	
};

static inline std::ostream& operator<<(std::ostream& out, const packet_data_args &value) {
    return value.print(out);
}

// exposed API to statistcs accumulator table
struct packet_data_container {
    packet_data_container(const packet_data_args &d) : data(d) {}
    uint64_t operator()(const Packet &value) const {
	uint64_t result;
        try{
		result = data(value);
	} catch (value_not_ready& e){
	
		throw value_not_ready(std::string(e.what()));
	}
	return result;
    }

    std::ostream& print(std::ostream& out) const {
        return data.print(out);
    }

   const packet_data_args &data;
};

static inline std::ostream& operator<<(std::ostream& out, const packet_data_container &value) {
    return value.print(out);
}

//struct packet_data_args {
//    packet_data_args() {}
//
//    virtual uint64_t operator()(const Packet &value) const = 0;
//    virtual std::ostream& print(std::ostream& out) const = 0;
//
//    std::string name;
//    std::string description;
//};
//
//struct packet_data : public packet_data_args {
//    std::function extract;
//    std::function print;
//
//    uint64_t operator(const Packet &p) const { extract(p) }
//}


// packet_extractor stuff

template<typename T>
struct packet_extractor {
    // removing const from packet_extractor
    packet_extractor(const var_template_type &var) : var_template(var) { }
    
    T operator()(Packet packet);
    
    operator const std::string & () const {
        return std::string("packet_extractor");
    }
private:
    const var_template_type &var_template;
};


// API for dominoes
template<>
struct packet_extractor<std::vector<uint64_t> > {
   struct var_template_visitor : public boost::static_visitor<>{
        var_template_visitor(std::vector<uint64_t> &v, const Packet &p) : vec(v), packet(p){ }
        void operator()(const packet_data_args &value) const {
		uint64_t result;
		try{
       	     		result = value(packet);
	     	} catch (value_not_ready& e){
			throw value_not_ready(std::string(e.what()));	
		}
	     	vec.push_back(result);
        }	
        void operator()(const packet_data_container &value) const {
		uint64_t result;
		try{
			result = value(packet);
		} catch (value_not_ready& e){
			throw value_not_ready(std::string(e.what()));
		}
                	vec.push_back(result);
        }
        void operator()(const std::string &value) const  {
        }
private:
        std::vector<uint64_t> &vec;
	const Packet &packet;	
        };

        packet_extractor(const var_template_type &var) : var_template(var) { }
    
        std::vector<uint64_t> operator()(Packet packet) { // TODO this could probably be marked const and be passed by reference
		std::vector<uint64_t> ret_vec;
        	for( auto &iter : var_template ) {
	    		try{
            			boost::apply_visitor(var_template_visitor(ret_vec, packet), iter);
            		} catch (value_not_ready& e) {
				ret_vec.clear();
				break;
	    		}
		 }
        	if(ret_vec.empty()){
            		ret_vec.push_back(0);
        	}
        	return ret_vec;
    	}

private:
    const var_template_type &var_template;
};

// Extracting the key
// accumulator_table creates a packet_extractor by passing to it the var_template_type object whcih in turn is of type
// packet_data_container which has packet_data_args which is the parent class of extractors
 
template<>
struct packet_extractor<std::string> {
    struct var_template_visitor : public boost::static_visitor<>{
        var_template_visitor(std::ostringstream &s, const Packet &p) : str(s), packet(p) { }
        void operator()(const packet_data_args &value) const {
		 uint64_t result;
		 try{
                 	result = value(packet);
	  	 } catch (value_not_ready& e){
                        throw value_not_ready(std::string(e.what()));
                 }
		str << result << " ";
        }
        void operator()(const packet_data_container &value) const{
		uint64_t result;
		try{
			result = value(packet);
        	} catch (value_not_ready& e){
                        throw value_not_ready(std::string(e.what()));
                } 
                        str << result << " ";
	
        }
        void operator()(const std::string &value) const {
            str << value << " ";
        }
    private:
        std::ostringstream &str;
        const Packet &packet;
    };
    packet_extractor(const var_template_type &var) : var_template(var) { }

    std::string operator()(Packet packet) { // TODO this could probably be marked const and be passed by reference
        std::ostringstream ret_str;
        for( auto &iter : var_template ) {
	    try{
            	 boost::apply_visitor(var_template_visitor(ret_str, packet), iter);
	    } catch (value_not_ready& e) {
		ret_str.str(std::string());
	//	std::cout <<"Error: " <<  e.what() << std::endl;
                break;
            }

	}
        return ret_str.str();
    }

private:
    const var_template_type &var_template;
};

#endif
