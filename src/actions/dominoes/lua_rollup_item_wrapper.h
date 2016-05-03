// Author: Subharthi Paul <subharpa@cisco.com>

// This is a wrapper for Lua to access the Rollup Item class

#ifndef LUA_ROLLUP_ITEM_WRAPPER_H
#define LUA_ROLLUP_ITEM_WRAPPER_H

#include <boost/preprocessor.hpp>

#include "lua/lua.h"
#include "lua/lua_util.h"
#include "accumulators/src/accumulator_group.hpp"
#include "dominoes_detector_api.h"
#include "observation.h"
#include "rollup/src/rollup.h"
#include <iostream>

struct DetectorUserData;
class Observation;

#include "accumulators/src/metrics.hpp"


#define CHECK_IF_REQUESTED_FEATURE_AVAILABLE(X) \
	bool flag = true; \
	if((X < 0) || (X >= BOOST_PP_SEQ_SIZE(METRICS_SEQ))) { \
		flag = false; \
	}   \
	return flag;


#define MACRO_FEATURE_RANGE(r, data, i , elem) \
	if(feature == i) return extract_result<tag::BOOST_PP_SEQ_ELEM(0,elem)>(data);

#define GENERATE_FEATURE_SELECTION_CODE(ACCUMULATOR_GRP)  BOOST_PP_SEQ_FOR_EACH_I(MACRO_FEATURE_RANGE,ACCUMULATOR_GRP, METRICS_SEQ)




//Print the macro expansion, if X is a macro, we expand it before printing it
#define PRINT(X) TO_STRING(X)
#define TO_STRING(X) #X


bool check_if_requested_feature_available(metric::metric_type feature);
int extract_result(metric::metric_type feature, accumulator_group<uint32_t>  accumulator_grp);

typedef std::pair <std::string, std::string > Key;
typedef std::vector<Key> KeyVec;
typedef std::pair<std::string, uint32_t> Result;
typedef std::vector<std::pair<KeyVec,Result> > ResultMap; 


class ResultTable {

private:
	ResultMap result_map;

public:
	//TODO: Error handling
	ResultMap& get_result_map() {return result_map;}
	void append(KeyVec _key, Result _result) {result_map.push_back(std::make_pair(_key, _result));}
	void print(){
		for (auto& result_pair: result_map){ 
			std::cout << "Keys: ";
			for (auto& key :result_pair.first){
					std::cout << "(" << key.first << " = " << key.second << ") ";
					
			}
			std::cout << " => " <<  "(" << result_pair.second.first << " = " << result_pair.second.second << ")" << std::endl; 	
			
		}	
	}
	
	void clear (){ 
		result_map.clear();
	}

};


// Create an instance of Item that is returned for all queries to the Rollup data
struct ItemWrapper{
	Item* item;
	DetectorUserData* detector_ref;	
	std::string observation_name;
	std::string detector_name;
	std::string accumulator_name;
	std::shared_ptr<Observation> observation;
	time_t start_time;
	time_t end_time;
	ResultTable result_table;

	void fill_result_table(metric::metric_type  feature); 
		
};
int range_query(lua_State *L);
int point_query_now(lua_State *L);
int rollup_item_lua_wrapper_new (lua_State *L);
int rollup_item_lua_wrapper_get_data(lua_State *L);
int rollup_item_lua_wrapper_get_interval(lua_State *L);
int rollup_item_lua_wrapper_get_end_time(lua_State *L);
int rollup_item_lua_wrapper_get_start_time(lua_State *L);

int register_rollup_item_wrapper(lua_State *L);
/*
static const luaL_reg Item[] = {
    {"new", rollup_item_lua_wrapper_new},
    {"range_query", range_query},
    {"get_data", rollup_item_lua_wrapper_get_data},
    {"get_st", rollup_item_lua_wrapper_get_start_time},
    {"get_et", rollup_item_lua_wrapper_get_end_time},
    {"get_interval", rollup_item_lua_wrapper_get_interval},
    {0,0}

};*/
#endif
