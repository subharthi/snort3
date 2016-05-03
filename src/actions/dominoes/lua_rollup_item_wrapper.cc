// Author: Subharthi Paul <subharpa@cisco.com>

#include "lua_rollup_item_wrapper.h"

// Create an instance of Item that is returned for al queries to the Rollup data
/*
int rollup_item_lua_wrapper_new(lua_State *L){
        Item **item_ptr = (Item**) lua_newuserdata(L, sizeof(Item*));
	*item_ptr = new Item();
        lua_getglobal(L, "Item"); // Use global table 'item' as metatable
        lua_setmetatable(L, -2);
	return 1;
}
*/

int rollup_item_lua_wrapper_new(lua_State *L){
        
	ItemWrapper** item_wrapper_ptr = (ItemWrapper**) lua_newuserdata(L, sizeof(ItemWrapper*));
	*item_wrapper_ptr = new ItemWrapper();
	(*item_wrapper_ptr)->item = new Item();
        /* Get the ref to the pDetector object */

	if(!lua_tonumber(L, -1)){
                std::cout<< "Lua Script Error: Item() first parameter is index to the LUA_REGISTRYINDEX"
                lua_error(L); 
        }
	 
        lua_rawgeti(L, LUA_REGISTRYINDEX,lua_tonumber(L, -1)) ;  
	(*item_wrapper_ptr)->detector_ref = check_detector_user_data(L, -1);
}

//TODO: Validate the function call and exit with error
int range_query(lua_State *L){


   	//TODO: Error check
   	// int metric = lua_tonumber(L,-1);
   	// std::string accumulator_table_name(strdup(lua_tostring(L,-1)));
    	std::string observation_name(strdup(lua_tostring(L,-1)));

   	 time_t et = time(NULL);
   	 time_t st = et - dominoes_tick.tv_sec;

   	 if (st < 0){
       	 	std::cout << " range too long "
                	  << std::endl;
        	return 0;
    	} else {
        	for(auto& iter_observation: detector_ref->pDetector->observation_list) {
                	if (iter_observation->get_observation_name() == observation_name ) {
                 		//     std::cout << "observation name:" <<  "\"" << observation_name << "\"" << "; " ;
                        	// Clear ref
                       		ref->clear(); 
                       		ref->setEndTime(et);
                       		ref->setStartTime(st);
                       		iter_observation->get_rollup().rangeQueryBottomUp(st, et, ref);
                        	return 0;
                	}
        	}
    }
}


int rollup_item_lua_wrapper_get_data(lua_State *L){

   if (!lua_touserdata(L, -3)){
    	std::cout << "Error!!" << std::endl;
	lua_error(L); // longjmp out.
   }
  Item* ref = *(Item**)lua_touserdata(L, -3);
  std::string accumulator_table_name(strdup(lua_tostring(L,-2)));
  
  metric::metric_type  feature = static_cast<metric::metric_type>(lua_tonumber(L,-1)) ; 
  // std::cout << metric << std::endl; 
 // lua_newtable(L);

  for ( auto &i : ref->getData()) {
                if(std::string(i.first)  == accumulator_table_name){
                        for( auto &k : i.second.data ) {
				std::cout << GET_TAG(1) << std::endl; 
                                int data =   extract_result<tag::count>(k.second) ;
				lua_pushnumber(L,data);
                        }
                        return 1;
                }

    }
  
   
   return 1;
} 

/*
namespace metric {
    enum feature_ {
        BOOST_PP_SEQ_ENUM(METRICS_SEQ_1),
        METRIC_TYPE_LAST,
    };


#define TAG_NAME() BOOST_PP_SEQ_ELEM(i,
(tag::count) (tag::mean)
#define f_1 tag::count
*/



int rollup_item_lua_wrapper_get_start_time(lua_State *L){
	
	if (!lua_touserdata(L, -1)){
     		std::cout << "Error!!" << std::endl;
      		lua_error(L); // longjmp out.
	}
	Item* ref = *(Item**)lua_touserdata(L, -1);
	lua_pushnumber(L,ref->getStartTime());
	return 1;
}


int rollup_item_lua_wrapper_get_end_time(lua_State *L){
	if (!lua_touserdata(L, -1)){
        	std::cout << "Error!!" << std::endl;
        	lua_error(L); // longjmp out.
   	}
   	Item* ref = *(Item**)lua_touserdata(L, -1);
	lua_pushnumber(L,ref->getEndTime());
	return 1;
}

int rollup_item_lua_wrapper_get_interval(lua_State *L){
        if (!lua_touserdata(L, -1)){
                std::cout << "Error!!" << std::endl;
                lua_error(L); // longjmp out.
        }
        Item* ref = *(Item**)lua_touserdata(L, -1);
        lua_pushnumber( L,ref->getEndTime() - ref->getStartTime() );
	return 1;
}




/*
static const luaL_reg rollup_item_lua_wrapper[] = {
    {"new", rollup_item_lua_wrapper_new},
    {"range_query", range_query},
    {"get_data", rollup_item_lua_wrapper_get_data},
    {"get_st", rollup_item_lua_wrapper_get_start_time},
    {"get_et", rollup_item_lua_wrapper_get_end_time},
    {"get_interval", rollup_item_lua_wrapper_get_interval},
    {0,0}

};*/

static const luaL_reg Item[] = {
    {"new", rollup_item_lua_wrapper_new},
    {"range_query", range_query},
    {"get_data", rollup_item_lua_wrapper_get_data},
    {"get_st", rollup_item_lua_wrapper_get_start_time},
    {"get_et", rollup_item_lua_wrapper_get_end_time},
    {"get_interval", rollup_item_lua_wrapper_get_interval},
    {0,0}

};

/*
int register_rollup_item_wrapper(lua_State *L) {

    luaL_register(L, "Item", rollup_item_lua_wrapper);
    lua_pushvalue(L,-1);
    lua_setfield(L, -2, "__index");

    return 1;   

}*/
