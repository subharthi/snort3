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

/* observation.h 
   Author:Subharthi Paul <subharpa@cisco.com>
*/

#ifndef OBSERVATION_HPP
#define OBSERVATION_HPP

#include <vector>
#include <string>
#include "sfip/sf_ip.h"
#include "dominoes_detector_api.h"
#include "accumulators/src/statistics.hpp"
#include "dominoes_detector.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <fstream>
#include <mutex>
#include "dominoes_globals.h"
#include "rollup/src/rollup.h"

// per thread event local sid
//thread_local uint32_t event_local_sid = 100000;

class Detector;

class EventFilter {
    std::string event_filter_rule_header;
    std::string event_filter_rule_options;
    std::string event_filter_rule_global_sid;
    uint32_t    event_filter_rule_local_sid;

    public:
    void        fill_configuration(lua_State *L);
    std::string get_ev_rule_header() {       return event_filter_rule_header; }
    std::string get_ev_rule_options() {      return event_filter_rule_options; }
    std::string get_ev_rule_global_sid() {   return event_filter_rule_global_sid; }
    uint32_t    get_ev_rule_local_sid() {    return event_filter_rule_local_sid; }

};

struct Key_Transform {
	std::string keystring;
	std::string mapper_conf[3];
	std::string meta_keystring;
	
	Key_Transform(std::string _keystring,std::string _mapper_conf[3], std::string _meta_keystring):
			 keystring(_keystring),
			 mapper_conf{_mapper_conf[0],_mapper_conf[1],_mapper_conf[2]},
		 	 meta_keystring(_meta_keystring){}
        //Copy constructor
        Key_Transform (const Key_Transform &kt ){
		keystring = kt.keystring;
		mapper_conf[0] = kt.mapper_conf[0];
		mapper_conf[1] = kt.mapper_conf[1];
		mapper_conf[2] = kt.mapper_conf[2];
		meta_keystring = kt.meta_keystring; 
	} 
	
	Key_Transform& operator=(const  Key_Transform& kt){
		keystring = kt.keystring;
                mapper_conf[0] = kt.mapper_conf[0];
                mapper_conf[1] = kt.mapper_conf[1];
                mapper_conf[2] = kt.mapper_conf[2];
                meta_keystring = kt.meta_keystring;	
	}
	std::string operator()(std::string _key){
		//TODO: Add error conditions
		
		if(mapper_conf[2] == std::string("file")){
			std::string key,val;
			std::ifstream mapping;
			mapping.open(mapper_conf[1], std::ios::in);
			if(!mapping){
				mapping.close();
				return std::string("no_map_file");
			}	
			while (mapping >> key >> val){
				if(key == _key ){
					mapping.close();
					return val;
				}
			} 	
			mapping.close();	

			return std::string("unknown"); 
		} else {

			return std::string("wrong_map_type");
		} 
	} 	
			 
		
};

/*
class Key_Transformer{
	std::string name;
	std::string type;
	std::string filepath;

        public:    
	key_transformer(std::string name, std::string type, std::string path): 
			name(name),
			type(type),
			path(path){};	
	operator()()

}
*/


class Observation
{
    std::string observation_name;
    Detector*   parent_detector;
    EventFilter event_filter;
    Statistics  statistics;
    std::vector<std::string> key_list;
    std::vector<std::string> metrics_list;
    RollupData  rollup;
    std::vector<Key_Transform> key_transform_list;  
    /** helpers*/
    void set_event_filter(lua_State* L);
    void set_observation_name(lua_State* L );
    void set_statistics(lua_State* L);
    void set_parent_detector(Detector* _parent_detector);

    void Log(std::string str);

    // Returns a unique identifier
    uint64_t get_uuid() {
        static uint64_t count = 0;
        return count++;
    }

    public:
    void fill_configuration(lua_State *L, const std::string &_obs_var_name, 
                            Detector* _parent_detector);
    EventFilter& get_event_filter() {        return event_filter; }
    std::string& get_observation_name() {    return observation_name; }
    Detector*   get_parent_detector() {     return parent_detector; }
    Statistics& get_statistics() {          return statistics; }
    RollupData& get_rollup(){ return  rollup; }
    std::vector<std::string>& get_key_list(){ return key_list;}
    std::vector<std::string>& get_metrics_list(){ return metrics_list;}
    std::vector<Key_Transform>& get_key_transform_list() {return key_transform_list;}
};   


#endif
