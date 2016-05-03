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



/*
 ** lua_detector_api.h  Author:Subharthi Paul <subharpa@cisco.com>
 ** Ported from the OpenAppID implementation in Snort and modified for Project Dominoes
 */

#ifndef _DOMINOES_DETECTOR_HPP_
#define  _DOMINOES_DETECTOR_HPP_

#include <sys/types.h>
#include <inttypes.h>
#include <vector>
#include <list>
#include <string>
#include <memory>
#include "lua/lua.h"
#include "lua/lua_util.h"

#include "hash/sfxhash.h"
#include "utils/sflsq.h"
#include "observation.h"
#include "dominoes_config.h"
#include "dominoes_metadata.h"

class Observation;
class Detector_Timed_Callback;
class Metadata_Source_Config;

struct DetectorPackageInfo
{
    std::string detector_name;
    std::string init_func;
    std::vector<std::string>  observation_list_names;
    int num_observations;
//  char* tick_callback_fn;
    std::vector<Metadata_Source_Config> metadata_source_config;
    std::vector<Detector_Timed_Callback> tick_callback_fn_list;
};

class Detector
{

    /**Package information retrieved from detector lua file.*/
    DetectorPackageInfo package_info;


    lua_State *L;

    /**Reference to lua userdata. This is a key into LUA_REGISTRYINDEX */
    int detector_user_data_ref;


    char *validator_buffer;
    unsigned char digest[16];


    /** helpers */	
    void set_lua_state(DominoesModuleConfig* _config);
    void set_detector_package_info();
    void set_detector_observation_list();
    void set_detector_metadata();
    void add_observation_to_dominoes_rules_file(std::shared_ptr<Observation> _observation);
    void Log(std::string str);

    public:
    
    Metadata_Acces_Object mao;
    //TODO: make this private
    std::vector<std::shared_ptr<Observation> > observation_list;

    /** Load the detector */
    void load_detector(DominoesModuleConfig* _config, std::string _detector_file_name);

    /* getters */
    std::string get_detector_name(){return package_info.detector_name;}
    lua_State* get_lua_state(){return L;}
    int get_detector_user_data_ref(){return detector_user_data_ref;}
    DetectorPackageInfo& get_detector_package_info(){return package_info; }
    std::vector<std::shared_ptr<Observation> >& get_observation_list(){return observation_list;}

    /* setters */
    void set_detector_user_data_ref(int _ref) {detector_user_data_ref = _ref;}
    
   /* callback init function in the detector script */
   void call_detector_init_func();

};

/* Lua Detector Callback functions */
class Detector_Timed_Callback
{
public:
        uint32_t count_duration;
        lua_State *L;
        uint32_t duration; /* number of tick in one duration */
        std::string name;

        Detector_Timed_Callback(){};
        Detector_Timed_Callback( char* name, uint32_t duration, lua_State * L);
        void call_detector_timed_callback_fn(std::shared_ptr<Detector> _detector);

};

/* Metadata source configurations */
class Metadata_Source_Config{
public:
	std::string metadata_name;
	std::string metadata_source;
	std::string metadata_source_type;	
};


#endif
