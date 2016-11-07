#ifndef _LUA_API_H_
#define _LUA_API_H_
#include "dominoes_globals.h"
#include <exception>

class bad_field_type : public std::exception {
    public:
    bad_field_type(std::string e) : expected(e) {}

    virtual const char* what() const throw() {
        return "bad type";
    }

    private:
    std::string expected;
};

static void lua_getfield_(lua_State* L, const std::string &field, std::string &var) {
    lua_getfield(L,-1, field.c_str());
    if (lua_isstring(L, -1)) {
        var = std::string(lua_tostring(L, -1));
    } else {
        throw bad_field_type("string");
    }

    lua_pop(L,1); // I don't know what this does
}

static void lua_getfield_(lua_State* L, const std::string &field, uint32_t &var) {
    lua_getfield(L,-1, field.c_str());
    if (lua_isnumber(L, -1)) {
        var = lua_tonumber(L, -1);
    } else {
        throw bad_field_type("number");
    }

    lua_pop(L,1); // I don't know what this does
}

static void lua_getfield_(lua_State* L, const std::string &field, std::vector<int> &var) {
    int a_size_v;

    lua_getfield(L,-1, field.c_str());
    if (!lua_istable(L, -1)) {
        throw bad_field_type("table");
    }

    a_size_v = luaL_getn(L, -1);
    for (int j=1; j <= a_size_v; j++) {
        lua_rawgeti(L,-1,j);
        if (!lua_isnumber(L, -1)) {
            throw bad_field_type("number");
        }
        var.push_back(lua_tonumber(L,-1));
        lua_pop(L,1);
    }
    lua_pop(L,1);
}


// TODO these will go away at some point
static void lua_getfield_(lua_State* L, const std::string &field, var_template_type &var) {
    int a_size_v;

    lua_getfield(L,-1, field.c_str());
    if (!lua_istable(L, -1)) {
        throw bad_field_type("table");
    }

    a_size_v = luaL_getn(L, -1);
    for (int j=1; j <= a_size_v; j++) {
        lua_rawgeti(L,-1,j);
        if (!lua_isnumber(L, -1)) {
             if (lua_isstring(L,-1)){
		// Allow for an empty string or any other string
		var.push_back("");
		lua_pop(L,1);
		break; 		
	     } else {
		throw bad_field_type("number");
	     }
	
        }
	std::cout << "Observation Field:" << get_extractor(lua_tonumber(L,-1)) << std::endl;
        var.push_back(get_extractor(lua_tonumber(L,-1)));
        lua_pop(L,1);
    }
    lua_pop(L,1);
}

static void lua_getfield_stringify_keys(lua_State* L, const var_template_type kv_vector, std::vector<std::string> &var) {
   
  for ( auto &key: kv_vector){
	//ADDED by staghavi@cisco.com, stringify and add the key, if and only if the key is not already in the vector
	if (!(std::find (var.begin(), var.end(), boost::get<packet_data_container>(key).data.stringify()) != var.end())){
	   var.push_back(boost::get<packet_data_container>(key).data.stringify());
	}
  } 
}

// TODO these will go away at some point
static void lua_getfield_(lua_State* L, const std::string &field, std::vector<metric::metric_type> &var) {
    int a_size_v;

    lua_getfield(L,-1, field.c_str());
    if (!lua_istable(L, -1)) {
        throw bad_field_type("table");
    }

    a_size_v = luaL_getn(L, -1);
    for (int j=1; j <= a_size_v; j++) {
        lua_rawgeti(L,-1,j);
        if (!lua_isnumber(L, -1)) {
            throw bad_field_type("number");
        }
	std::cout << "Metrics: "<< lua_tonumber(L,-1) << std::endl;
        var.push_back(static_cast<metric::metric_type>(lua_tonumber(L,-1)));
        lua_pop(L,1);
    }
    lua_pop(L,1);
}


static void lua_getfield_stringify_features(lua_State* L, const std::vector<metric::metric_type> feature_list, std::vector<std::string> &var) {

    for (auto & feature: feature_list ){
    	std::cout << "Metrics: "<< metric::metric_type_decoder(feature) << std::endl;
	//ADDED by staghavi@cisco.com, stringify and add the feature, if and only if the feature is not already in the vector
	if (!(std::find (var.begin(), var.end(), metric::metric_type_decoder(feature)) != var.end())){
	     var.push_back(metric::metric_type_decoder(feature));
	}
    }
}


#endif /* _LUA_API_H_ */
