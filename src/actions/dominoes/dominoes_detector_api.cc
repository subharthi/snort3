/*
 ** Copyright (C) 2014 Cisco and/or its affiliates. All rights reserved.
 ** Copyright (C) 2005-2013 Sourcefire, Inc.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License Version 2 as
 ** published by the Free Software Foundation.  You may not use, modify or
 ** distribute this program under any other version of the GNU General
 ** Public License.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


/** @defgroup LuaDetectorBaseApi LuaDetectorBaseApi
 * This module supports basic API towards Lua detectors.
 *@{
 */

/*
 ** lua_detector_api.cc  Author:Subharthi Paul <subharpa@cisco.com>
 ** Ported from the OpenAppID implementation in Snort and modified for Project Dominoes
 */



#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <inttypes.h>
#include <assert.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include "dominoes_detector_api.h"
#include "observation.h"
//#include "lua_rollup_item_wrapper.h"
#include "log/messages.h"
#include "lua/lua.h"
#include "lua/lua_util.h"
#include "lua/lua_stack.h"
#include "lua/lua_table.h"
#include <luajit-2.0/lua.hpp>
#include <memory>

#include <boost/typeof/typeof.hpp>
#include <boost/typeof/std/vector.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/assign/list_of.hpp>
#include <tuple>


DetectorUserData *pushDetectorUserData(lua_State *L, std::shared_ptr<Detector> _detector)
{
	DetectorUserData *bar = (DetectorUserData *)lua_newuserdata(L, sizeof(DetectorUserData));

	if (bar) {
#ifdef LUA_DETECTOR_DEBUG
		LogMessage("DetectorUserData %p: allocated\n\n", bar);
#endif
		memset(bar, 0, sizeof(*bar));

		bar->pDetector = _detector;

		luaL_getmetatable(L, DETECTOR_API);
		lua_setmetatable(L, -2);

#ifdef LUA_DETECTOR_DEBUG
		LogMessage(DEBUG_LOG,"Detector %p: allocated\n\n", bar->pDetector);
#endif
	}
	return bar;
}

DetectorUserData *check_detector_user_data (lua_State *L,int index)
{
    DetectorUserData *bar;

    luaL_checktype(L, index, LUA_TUSERDATA);

    bar = (DetectorUserData *)luaL_checkudata(L, index, DETECTOR_API);
    if (bar == NULL) {
        luaL_typerror(L, index, DETECTOR_API);
    }

    return bar;
}

/*
static int print_stats (lua_State *L){
	// Get the parameters
	DetectorUserData *user_data;
	int detector_user_date_ref = lua_tonumber(L, 1);
	lua_rawgeti(L, LUA_REGISTRYINDEX, detector_user_date_ref);
	
	user_data = check_detector_user_data(L, -1);
	
	for(auto& iter_observation : user_data->pDetector->observation_list) {
		accumulator_table_map_type table = iter_observation->get_statistics().dump();
		for( auto &i : table ) {
			std::cout << "\"" << i.first << "\": ";
			std::cout << i.second << std::endl;
		}
	}	
	return 0;

}
*/

bool check_if_requested_feature_available(metric::metric_type feature){
        CHECK_IF_REQUESTED_FEATURE_AVAILABLE(feature)
};

int extract_result(metric::metric_type feature, accumulator_group<uint32_t>  accumulator_grp){
        GENERATE_FEATURE_SELECTION_CODE(accumulator_grp)
};



//TODO: Validate the function call and exit with error
static int print_stats(lua_State *L){

    DetectorUserData *user_data;
    //TODO: Error check
    std::string accumulator_table_name(strdup(lua_tostring(L,-1)));
    std::string observation_name(strdup(lua_tostring(L,-2)));
    int detector_user_date_ref = lua_tonumber(L, -3);

    lua_rawgeti(L, LUA_REGISTRYINDEX, detector_user_date_ref);
    user_data = check_detector_user_data(L, -1);
    std::cout << "Inside Get stats: "  << std::endl;
    for(auto& iter_observation:  user_data->pDetector->observation_list) {
	if (iter_observation->get_observation_name() == observation_name ) {
		std::cout << "observation name:" <<  "\"" << observation_name << "\"" << "; " ;
		iter_observation->get_rollup().print();
		std::cout << std::endl;
		return 0;
	}
    }	
/*
    for(auto& iter_observation:  user_data->pDetector->observation_list) {
        if (iter_observation->get_observation_name() == observation_name ) {
            std::cout << "observation name:" <<  "\"" << observation_name << "\"" << "; " ;
            accumulator_table_map_type table = iter_observation->get_statistics().dump();
            for ( auto &i : table ) {
                // only fetch the specified accumulator table
                if (i.first == accumulator_table_name){
                    std::cout << "accumulator_name:" <<  "\"" << i.first << "\": ";
                    try {
                        for( auto &k : i.second.data ) {
                            //std::cout << "\"" << i.first << "\": ";
                            std::cout << "  \"" << k.first << "\":  ";
                           // std::cout << k.second;
                            std::cout << extract_result<tag::count>(k.second) << std::endl;
                        }
                    } catch (std::exception &e) {
                        std::cout << "accumulator table: " << accumulator_table_name
                            << " for observation: " << observation_name
                            << " Detector: " << user_data->pDetector->get_detector_name()
                            <<   "doesn't exist" << std::endl;
                        exit (-1);
                    }
                }
            }	
            return 0;
        }
    } */
    std::cout << "Error: Observation name: " << observation_name 
              << " not specified in detector: " << user_data->pDetector->get_detector_name() 
              << std::endl; 
    exit(-1);

}

static int rollup_item_lua_wrapper_print_observation(lua_State *L)
{
  	ItemWrapper* item_wrapper_ref = *(ItemWrapper**)lua_touserdata(L, -3);
	std::string accumulator_table_name(strdup(lua_tostring(L,-2)));
	metric::metric_type  feature = static_cast<metric::metric_type>(lua_tonumber(L,-1)) ;

	item_wrapper_ref->item->print_accumulator(accumulator_table_name, feature);
	return 0;
	/*
	for ( auto &i : item_wrapper_ref->item->getData()) {
             if(std::string(i.first)  == accumulator_table_name){
                        for( auto &k : i.second.data ) {
                                item_wrapper_ref->print_accumulator()  //  int data = extract_result(feature, k.second);
                                lua_pushnumber(L,data);
                        }
                        return 1;
                }
    }
   return 1;

        for(auto& iter_observation: item_wrapper_ref->item->detector_ref->pDetector->observation_list) {
                               item_wrapper_ref->print_accumulator(accumulator_table_name, feature) ;
                               return 0;
        }*/
}
/*
struct zip_keystring_with_key: public std::unary_function<const boost::tuple<const std::string&, const std::string&>&, void> 
{
	zip_keystring_with_key() {}
	void operator()(const boost::tuple<const std::string&, const std::string &>& t){
        	std::cout << t.get<0>() << ":" << t.get<1>();
	}
};
*/
static std::vector<std::string>  tokenize_key(std::string key){
        std::vector<std::string> tokens;
	std::istringstream iss(key);  
        copy(std::istream_iterator<std::string>(iss),
                std::istream_iterator<std::string>(),
                back_inserter(tokens)); 
	return tokens;
	
}

static std::string stringify_key(std::string key, std::string label)
{

    if (label.find("ip")){
	// == "src_ip" || label == "dst_ip" || label == "client_ip" ){	
    	std::istringstream strm(key);
    	char buffer[16];
    	uint64_t ip;
    	strm >> ip;
    	unsigned char bytes[8];
    	bytes[3] = ip & 0xFFFF;
    	bytes[2] = (ip >> 8) & 0xFFFF;
    	bytes[1] = (ip >> 16) & 0xFFFF;
   	 bytes[0] = (ip >> 24) & 0xFFFF;	
    	sprintf(buffer, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);        
   	return std::string(buffer);
    } else {
	return key;
    }

	
}


static int rollup_item_print_like_json(lua_State *L){
	std::ostringstream json_string;
	//TODO: error check        
	ItemWrapper* item_wrapper_ref = *(ItemWrapper**)lua_touserdata(L, -1);
        
	for (auto& result_pair: item_wrapper_ref->result_table.get_result_map()){
                        json_string << "{" << std::endl 
				    << "script: " << item_wrapper_ref->detector_name << std::endl 
				    << "observation: " << item_wrapper_ref->observation_name << std::endl
				    << "accumulator_name: " << item_wrapper_ref->accumulator_name << std::endl
				    << "start_time: " << item_wrapper_ref->item->getStartTime() << std::endl	
				    << "end_time: " << item_wrapper_ref->item->getEndTime() << std::endl;	
                        for (auto& key :result_pair.first){
                                        json_string << key.first << ": " << key.second << std::endl;
                        }
                        json_string  << result_pair.second.first << ": " << result_pair.second.second << std::endl << "}" << std::endl;

         }
      std::cout << json_string.str();
      return 0;
}


int rollup_item_lua_wrapper_new(lua_State *L){

	int detector_user_data_ref;
	std::string observation_name;
	std::string accumulator_name;	
        
	/* Get the ref to the pDetector object */
	if(lua_isstring(L,-1)){
		accumulator_name = std::string(strdup(lua_tostring(L,-1)));
	} else {
		std::cout<< "Lua Script Error: usage: Item(detector_ref, observation_name, accumulator_name)" << std::endl;	
		lua_error(L);
	}
	
	if(lua_isstring(L,-2)){
       		observation_name = std::string(strdup(lua_tostring(L,-2)));
	 } else {
                std::cout<< "Lua Script Error: usage: Item(detector_ref, observation_name, accumulator_name)" << std::endl;
                lua_error(L);
        }
        
	if(lua_isnumber(L, -3)){
		detector_user_data_ref = lua_tonumber(L, -3);
	} else {
                std::cout<< "Lua Script Error: Item() first parameter is index to the LUA_REGISTRYINDEX" << std::endl;
                lua_error(L);
        }
	
        lua_rawgeti(L, LUA_REGISTRYINDEX, detector_user_data_ref) ;
        DetectorUserData* detector_ref = check_detector_user_data(L, -1);
        
	ItemWrapper** item_wrapper_ptr = (ItemWrapper**) lua_newuserdata(L, sizeof(ItemWrapper*));
        *item_wrapper_ptr = new ItemWrapper();
        (*item_wrapper_ptr)->item = new Item();
	(*item_wrapper_ptr)->detector_ref = detector_ref;
        (*item_wrapper_ptr)->observation_name  = observation_name;
	(*item_wrapper_ptr)->accumulator_name = accumulator_name;  
        (*item_wrapper_ptr)->detector_name = detector_ref->pDetector->get_detector_package_info().detector_name;
	
	// set a pointer to the observation object 
	for(auto& observation_iter: detector_ref->pDetector->observation_list) {
                if (observation_iter->get_observation_name() == observation_name ) {
                        (*item_wrapper_ptr)->observation = observation_iter;
                        break;
                }
        }
	lua_getglobal(L, "Item"); // Use global table 'item' as metatable
        lua_setmetatable(L, -2);	
  	return 1;      
}

/*
static void return_item_as_lua_table(lua_State *L, Item* item){
	//create the outer table
	//lua_createtable(L,0,);
	//lua_pushstring(L, item_wrapper_ref->item->getStartTime());
	//lua_setfield(L, -2, "start_time");
	
	//lua_pushstring(L, item_wrapper_ref->item->getEndTime());
	//lua_setfield(L, -2, "end_time");
	
	int num_rows = item_wrapper_ref->item->getData().size();
	lua_createtable(L,0,num_rows);
	
	

	
}
*/

void ItemWrapper::fill_result_table(metric::metric_type feature){

 	std::vector<std::string> key_list_temp =  observation->get_key_list();
        std::vector<Key_Transform>  key_transform_list_temp = observation->get_key_transform_list();
        std::vector<std::string> metric_list_temp = observation->get_metrics_list();

	// clear the table
        result_table.clear();

        for (auto &i : item->getData()) {
        	if(std::string(i.first) == accumulator_name){
                	for( auto& k : i.second.data ) {
				KeyVec temp_key_vec;
				Result temp_result;
                         	std::vector<std::string> tokens;
                                tokens = tokenize_key(k.first);
                                for (int j = 0; j< tokens.size(); j++){
                                	Key primary_key_temp;
                                        primary_key_temp.first = key_list_temp[j];
                                        primary_key_temp.second = stringify_key(tokens[j], key_list_temp[j]);
                                        temp_key_vec.push_back(primary_key_temp);
                                        if(key_transform_list_temp.size()){
                                        	for (auto& transform: key_transform_list_temp){
                                                 	if(transform.keystring == key_list_temp[j]){
                                                        	Key derived_key_temp;
                                                                derived_key_temp.first = transform.meta_keystring;
                                                                derived_key_temp.second = transform(primary_key_temp.second);
                                                                temp_key_vec.push_back(derived_key_temp);
                                                          }
                                                 }
                                         }

                                }
				// metrics_list_temp only has the accumulators specified in the accumulator_group constuctor; hence,
                                // we need to find the right accumulator by checking the decoded feature against elements of
				// the metrics_list_temp, modified by: staghavi@cisco.com
				for (int l = 0; l< metric_list_temp.size(); l++){
				    if (metric::metric_type_decoder(feature) == metric_list_temp[l]) {
					temp_result.first = metric_list_temp[l];
                                        temp_result.second = extract_result(feature, k.second);
                                        result_table.append(temp_key_vec, temp_result);
				    }
				}
                         }
                }

         }

}

//TODO: Validate the function call and exit with error
int range_query(lua_State *L){


        //TODO: Error check
        metric::metric_type  feature = static_cast<metric::metric_type>(lua_tonumber(L,-1)) ;
	ItemWrapper* item_wrapper_ref = *(ItemWrapper**)lua_touserdata(L, -2);
     
	
         time_t et = time(NULL);
         time_t st = et - dominoes_tick.tv_sec;
         if (st < 0){
                std::cout << " range too long "
                          << std::endl;
                return 0;
        } else {
                                // Clear ref
                                item_wrapper_ref->item->clear();
                                item_wrapper_ref->item->setEndTime(et);
                                item_wrapper_ref->item->setStartTime(st);
		                item_wrapper_ref->observation->get_rollup().query(st, et, item_wrapper_ref->item);
				// fillup the result_map
				item_wrapper_ref->fill_result_table(feature);
				//item_wrapper_ref->result_table.print();
                                return 0;
        }
}

//ADDED by staghavi@cisco.com: Overloaded range_query capable of passing number of dominoes_tick before you want to query the rollup for 
int range_query_duration(lua_State *L){


        //TODO: Error check
        uint32_t numberofpastticks = uint32_t(lua_tonumber(L,-1));
        metric::metric_type  feature = static_cast<metric::metric_type>(lua_tonumber(L,-2)) ;
        ItemWrapper* item_wrapper_ref = *(ItemWrapper**)lua_touserdata(L, -3);


         time_t et = time(NULL);
         time_t st = et - (numberofpastticks * dominoes_tick.tv_sec);
         if (st < 0){
                std::cout << " range too long "
                          << std::endl;
                return 0;
        } else {
                                // Clear ref
                                item_wrapper_ref->item->clear();
                                item_wrapper_ref->item->setEndTime(et);
                                item_wrapper_ref->item->setStartTime(st);
                                item_wrapper_ref->observation->get_rollup().rangeQueryBottomUp(st, et, item_wrapper_ref->item);
                                // fillup the result_map
                                item_wrapper_ref->fill_result_table(feature);
                                //item_wrapper_ref->result_table.print();
                                return 0;
        }
}
int point_query_now(lua_State *L){
	
	metric::metric_type  feature = static_cast<metric::metric_type>(lua_tonumber(L,-1)) ;
	ItemWrapper* item_wrapper_ref = *(ItemWrapper**)lua_touserdata(L, -2);
	time_t et = time(NULL);
	time_t st = et - dominoes_tick.tv_sec;
	for(auto& iter_observation: item_wrapper_ref->detector_ref->pDetector->observation_list) {
                        if (iter_observation->get_observation_name() == item_wrapper_ref->observation_name ) {
                                // Clear ref
                                item_wrapper_ref->item->clear();
                                item_wrapper_ref->item->setEndTime(et);
                                item_wrapper_ref->item->setStartTime(st);
                                iter_observation->get_rollup().query(et, item_wrapper_ref->item);
                          	item_wrapper_ref->fill_result_table(feature);
				item_wrapper_ref->result_table.print();
			        return 0;
                        }
                }
	
}


int rollup_item_lua_wrapper_get_data(lua_State *L){

   if (!lua_touserdata(L, -3)){
        std::cout << "Get data fn: Error!!" << std::endl;
        lua_error(L); // longjmp out.
   }
  ItemWrapper* item_wrapper_ref = *(ItemWrapper**)lua_touserdata(L, -3);
  std::string accumulator_table_name(strdup(lua_tostring(L,-2)));
  metric::metric_type  feature = static_cast<metric::metric_type>(lua_tonumber(L,-1)) ;
 // std::cout << PRINT(CHECK_IF_REQUESTED_FEATURE_AVAILABLE(feature)) << std::endl; 
 // std::cout << PRINT(GENERATE_FEATURE_SELECTION_CODE(k.second)) <<  std::endl;

  if(!check_if_requested_feature_available(feature)){
	//Feature asked for is not supported
 	//TODO: Add script name to error message
	std::cout << "Lua Script Error: Requesting unsupported feature.. Exiting Snort" << std::endl;
	exit(-1);	
  } 
  for ( auto &i : item_wrapper_ref->item->getData()) {
             if(std::string(i.first)  == accumulator_table_name){
                        for( auto &k : i.second.data ) {
                                int data = extract_result(feature, k.second);
				lua_pushnumber(L,data);
                        }
                        return 1;
                }
    }
   return 1;
}


int rollup_item_lua_wrapper_get_start_time(lua_State *L){

        if (!lua_touserdata(L, -1)){
                std::cout << "Error!!" << std::endl;
                lua_error(L); // longjmp out.
        }
        ItemWrapper* item_wrapper_ref = *(ItemWrapper**)lua_touserdata(L, -1);
	lua_pushnumber(L,item_wrapper_ref->item->getStartTime());
        return 1;
}


int rollup_item_lua_wrapper_get_end_time(lua_State *L){
        if (!lua_touserdata(L, -1)){
                std::cout << "Error!!" << std::endl;
                lua_error(L); // longjmp out.
        }
        ItemWrapper* item_wrapper_ref = *(ItemWrapper**)lua_touserdata(L, -1);
	lua_pushnumber(L,item_wrapper_ref->item->getEndTime());
        return 1;
}

int rollup_item_lua_wrapper_get_interval(lua_State *L){
        if (!lua_touserdata(L, -1)){
                std::cout << "Error!!" << std::endl;
                lua_error(L); // longjmp out.
        }
        ItemWrapper* item_wrapper_ref = *(ItemWrapper**)lua_touserdata(L, -1);
	lua_pushnumber( L,item_wrapper_ref->item->getEndTime() - item_wrapper_ref->item->getStartTime() );
        return 1;
}






static const luaL_reg Output[] = {
    {"print_stats", print_stats},
  //  {"get_stats", get_stats},
 //   {"range_query_last_n", range_query_last_n},
 //   {"query_current_window", query_current_window},
    {0,0}
};

static const luaL_reg Query[] = {

  // {"range_query_last_n", range_query_last_n},
   {0,0}
};

static const luaL_reg detector_api[] = {
	{0,0}
};

static const luaL_reg Item[] = {
    {"new", rollup_item_lua_wrapper_new},
    {"range_query", range_query},
    {"range_query_duration", range_query_duration},
    {"point_query_now", point_query_now},
    {"get_data", rollup_item_lua_wrapper_get_data},
    {"get_st", rollup_item_lua_wrapper_get_start_time},
    {"get_et", rollup_item_lua_wrapper_get_end_time},
    {"get_interval", rollup_item_lua_wrapper_get_interval},
    {"print_observation", rollup_item_lua_wrapper_print_observation},
    {"print_like_json",rollup_item_print_like_json},
    {0,0}

};

/*
static const luaL_reg Item_m[] = {
	{"__call", rollup_item_lua_wrapper_new},
	{0,0}
};
*/

/**Registers C functions as an API, enabling Lua detector to call these functions. This function
 * should be called once before loading any lua detectors. This function itself is not part of API
 * and therefore can not be called by a Lua detection.
 *
 * @param Lua_State* - Lua state variable.
 * @param detector/stack - detector object
 * @return int - Number of elements on stack, which is 1 if successful, 0 otherwise.
 * @return methodArray/stack - array of newly created methods
 */
int register_detector_apis (lua_State *L)
{

    /* populates a new table with detector_api (method_table), add the table to the globals and stack*/
  //  luaL_openlib(L, DETECTOR_API, Query, 0);

    /* create new metatable */
   // luaL_newmetatable(L, Query);
   // lua_pop(L, 1);   

    /* populates a new table with detector_api (method_table), add the table to the globals and stack*/
   // luaL_openlib(L, DETECTOR_API, Output, 0);

    /* create new metatable */
  //  luaL_newmetatable(L, Output);
  //  lua_pop(L, 1);
     
    luaL_openlib(L, DETECTOR_API, detector_api, 0);
  //   luaL_newmetatable(L, Output);
  //    lua_pop(L, 1);
    luaL_newmetatable(L, DETECTOR_API);
    lua_pop(L, 1); 
    return 1;  /* return methods on the stack */


}

int register_item_apis(lua_State *L){


	luaL_register(L, "Item", Item);
 	// make a copy of the top of the stack	
    	lua_pushvalue(L,-1);
	// Set the metatable __index to the table itself
   	lua_setfield(L, -2, "__index");
//	lua_pop(L,1);
//        luaL_register(L, "Item_m", Item_m);
//	luaL_register(L, "Item", Item); 
	//luaL_newmetatable(L,"Item_m");
//	lua_setfield(L, -1, "__index");
//	lua_setmetatable(L, -2);
	
//	lua_pushcfunction(L, rollup_item_lua_wrapper_new);
//	lua_setfield(L, -2, "__call"); 
	
	
//	lua_pushvalue(L,-1);
//	lua_setfield(L, -2, "__index");
     
        //lua_getfield(L, -1, "new");
	//lua_setfield(L, -2, "__call");	
 
        lua_pop(L,1);	
    	
	return 1;   

}


/*
Detector *createDetector(
		lua_State *L,
		const char *chunkName
		)
{
	DetectorUserData *pUserData;
	Detector  *detector;

	pUserData = pushDetectorUserData(L);

	if (!pUserData || !pUserData->pDetector)
	{
		LogMessage( "Failed to allocate memory.");
		return NULL;
	}

	detector = pUserData->pDetector;

	lua_pushvalue( L, -1 );     // create a copy of userData 
	detector->detectorUserDataRef = luaL_ref( L, LUA_REGISTRYINDEX );
	detector->chunkName = strdup(chunkName);
	if (!detector->chunkName)
	{
		free(pUserData->pDetector);
		return NULL;
	}

	detector->myLuaState = L;

	return detector;
}*/

