//--------------------------------------------------------------------------
// Copyright (C) 2014-2015 Cisco and/or its affiliates. All rights reserved.
// Copyright (C) 2005-2013 Sourcefire, Inc.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------

/** observation.cpp Author:Subharthi Paul <subharpa@cisco.com> */


#include "observation.h"
#include "main/snort_types.h"
#include "main/snort_debug.h"
#include "main/snort_config.h"
#include "accumulators/src/statistics.hpp"
#include "accumulators/extractors/real_extractors.hpp"
#include "dominoes_globals.h"
#include <iostream>

#include "lua_api.h"

void Observation::Log(std::string str) {
    LogMessage("\nError in script: %s, observation:%s: %s\n", 
                get_parent_detector()->get_detector_name().c_str(), 
                get_observation_name().c_str(), str.c_str());
}

void Observation::fill_configuration(lua_State* L, const std::string &_obs_var_name, Detector* _parent_detector) {

    lua_getglobal (L,_obs_var_name.c_str());
    if (!lua_istable(L, -1)) {
        lua_pop(L,1);
        std::cout << "CONFIG ERROR:Observation: " << _obs_var_name 
                  << "in Detector: " << get_parent_detector()->get_detector_name() 
                  << "needs to be a Lua Table" << std::endl;
        exit(-1);
    }

    lua_getfield(L, -1, "name"); // string 
    if (lua_isstring(L, -1)) {
        //TODO  Replace blank space with - in the name
        observation_name = std::string(strdup(lua_tostring(L, -1)));
    } else  {
        // generate a name
        std::cout<< "generating a name for the observation" << std::endl;
        observation_name = get_parent_detector()->get_detector_name() + std::string("_") + std::to_string(get_uuid());
    }
    lua_pop(L, 1);


    //fill the fields
    set_observation_name(L);
    set_parent_detector(_parent_detector);	
    event_filter.fill_configuration(L);

    //TODO: Remove hard coding from this rollup code
    rollup.setNoOfLevels(2);
    Level* level1 = new Level(10000,5);
    Level* level2 =  new Level(10000,6);
    rollup.addLevel(level1);
    rollup.addLevel(level2);

    set_statistics(L);

}

void Observation::set_parent_detector( Detector* _parent_detector) {
    parent_detector = _parent_detector;
}

void Observation::set_observation_name(lua_State* L ) {
    try {
        lua_getfield_(L,"name", observation_name);
    } catch(...) {
        Log("generating a name for the observation");
        observation_name = get_parent_detector()->get_detector_name() + std::string("_") + std::to_string(get_uuid());
    }
}

void Observation::set_statistics(lua_State* L) {

    // get the list of statistics for this observation
    lua_getfield(L, -1, "statistics_list"); //table
    if (lua_istable(L, -1)) {
        int size = luaL_getn(L, -1);
        // Allocate flow selector_list
        //TODO: Error handling
        for (int i=1; i<= size; i++) {
            std::string name;
            var_template_type key_vector;
            var_template_type value_vector;
            int a_size_v;

            lua_rawgeti(L,-1,i);

            try {
                lua_getfield_(L,"name", name);
            } catch(...) {
                Log("Statistics name must be a string");
                exit(-1);
            }

            try {
                lua_getfield_(L,"key", key_vector);
		lua_getfield_stringify_keys(L,  key_vector, key_list);
            } catch(...) {
                Log("key must be a table");
                exit(-1);
            }

            try {
                lua_getfield_(L,"features", feature_list);
		lua_getfield_stringify_features(L,feature_list, metrics_list);
            } catch(...) {
                Log("key must be a table");
                exit(-1);
            }

            uint32_t temp_value = 0;
            try {
                lua_getfield_(L,"value", value_vector);
            } catch(...) {
                Log("value must be a table");
                exit(-1);
            }
 	    /*
            try {
                value_vector.push_back(get_extractor(temp_value));
            } catch(...) {
                Log("Invalid packet extractor for value.");
                exit(-1);
            }
	   */
            get_statistics().add(name, key_vector, value_vector, feature_list);
	
 	    //TODO move this to factored code
           std::string keystring;
	   std::string meta_keystring;	
	   std::string mapper_conf[3];
	   
           lua_getfield(L, -1, "key_transforms");
	   if (lua_istable(L, -1)) {
	   	std::cout << "key_transforms : ";  
	   	int key_transform_table_size = luaL_getn(L, -1);
    	   	for (int j=1; j <= key_transform_table_size ; j++) {
			// transform table element	 
	         	lua_rawgeti(L,-1,j);
       			 if (lua_istable(L, -1)) {
				// get the elements of the transform element table
                		lua_rawgeti(L,-1,1);
                		// First element is the key id
                		if(lua_isnumber(L, -1)){
					 int key =  lua_tonumber(L,-1);
       					 keystring = get_extractor(key).stringify();
                			 lua_pop(L,1);
				} else{
					throw bad_field_type("number");
				}
			         
     				// get the maping service config
				lua_rawgeti(L,-1,2);
				if(lua_istable(L, -1)){
					for (int k= 1; k<=3; k++){
						lua_rawgeti(L, -1, k);
                                        	if(lua_isstring(L, -1)){
                                           	     mapper_conf[k-1] = std::string(strdup(lua_tostring(L, -1)));
                                        	}else{
                                                	throw bad_field_type("string");
                                        	}
                                        	lua_pop(L,1);		
					}
				
					lua_pop(L,1);	
					
				} else {
					throw bad_field_type("table");	
				}		
				
				// get the mapping service config
				lua_rawgeti(L,-1,3);
                		if(lua_isstring(L, -1)){
					meta_keystring =  std::string(strdup(lua_tostring(L, -1)));
		                	lua_pop(L,1);

				} else {
                        		throw bad_field_type("string");
				}
                	} else{
				throw bad_field_type("table");
			}
			// Pop the transform table element
			lua_pop(L,1);

			//set a key transform node
			key_transform_list.push_back(Key_Transform(keystring,mapper_conf,meta_keystring));
			
         	} 
	    } else {
	 	   throw bad_field_type("table");
	    }
	  // pop the key transform table	
	  lua_pop(L,1);
	  
	   		
      //pop the statistcis object from the statistics list	
      lua_pop(L,1);
     }
 } else {
    //TODO: add a default selector
    Log("No statstics specified for this");
    exit(-1);
 }
 lua_pop(L,1);

}

void EventFilter::fill_configuration(lua_State* L){

    try {
        lua_getfield_(L,"event_filter_rule_header", event_filter_rule_header);
    } catch(...) {
        LogMessage("Error: No Rule header in dominoes script");
        exit(-1);
    }

    try {
        lua_getfield_(L,"event_filter_rule_options", event_filter_rule_options);
    } catch(...) {
        exit(-1);
    }
    //TODO: check for ; in the rule options
    /*	
        if (!(event_filter.event_filter_rule_options [strlen(event_filter.event_filter_rule_options)-1] == ';' )){
        event_filter.event_filter_rule_options = strcat (event_filter.event_filter_rule_options, ";");
        } */		

    try {
        lua_getfield_(L,"event_filter_rule_global_sid", event_filter_rule_global_sid);
    } catch(...) {
        LogMessage("\nError: No sid specified for rule in dominoes script\n");
        exit(-1);
    }
    event_filter_rule_local_sid = ++event_local_sid;

}

