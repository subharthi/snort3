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
 ** dominoes_detector.cpp  Author:Subharthi Paul <subharpa@cisco.com>
 */

#include "dominoes_detector.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <glob.h>
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "main/snort_types.h"
#include "main/snort_debug.h"
#include "main/snort_config.h"
//#include "time/profiler.h"
#include "stream/stream_api.h"
#include "file_api/file_api.h"
#include "parser/parser.h"
#include "framework/ips_action.h"
#include "utils/sfsnprintfappend.h"
#include "target_based/snort_protocols.h"
#include "managers/inspector_manager.h"
#include "framework/ips_action.h"
#include "lua/lua.h"
#include "lua/lua_util.h"
#include "sfip/sf_ip.h"

//#include "lua_rollup_item_wrapper.h"
#include "dominoes_detector_api.h"
#include "dominoes_module.h"
#include "dominoes_metadata.h"
#include "hash/sfxhash.h"
#include "utils/sflsq.h"
//#include "utils/sf_sechash.c"
#include "detection/treenodes.h"
#include "accumulators/src/statistics.hpp"
#include "accumulators/extractors/real_extractors.hpp"
#include "dominoes_module.h"
#include "dominoes_macros.h"
#include "dominoes_globals.h"

#include "lua_api.h"

#ifdef HAVE_OPENSSL_MD5
#include <openssl/md5.h>

#define MD5CONTEXT MD5_CTX

#define MD5INIT    MD5_Init
#define MD5UPDATE  MD5_Update
#define MD5FINAL   MD5_Final
#define MD5DIGEST  MD5

#else
#include "md5.h"
#define MD5CONTEXT struct MD5Context

#define MD5INIT   MD5Init
#define MD5UPDATE MD5Update
#define MD5FINAL  MD5Final
#define MD5DIGEST MD5
#endif

#define MAXPD 1024

void Detector::Log(std::string str) {
    LogMessage("\nError: %s %s\n", 
                str.c_str(), get_detector_name().c_str());
}

void Detector::load_detector(DominoesModuleConfig* _config, std::string _detector_file_name) {

    FILE *file;
    uint8_t* validator_buffer;
    size_t validator_buffer_len;
    MD5CONTEXT context;

    // Convert to ifstream
    if ((file = fopen(_detector_file_name.c_str() , "r")) == NULL){
        fclose(file); 
        LOG_LUA_ERROR_WITH_FILENAME_AND_EXIT(L,_detector_file_name, "Creating Dominoes Lua Module: ERROR: Unable to read Lua detector file");
    }

    // Load lua file as a detector.
    if (fseek(file, 0, SEEK_END)){
        fclose(file);
        LOG_LUA_ERROR_WITH_FILENAME_AND_EXIT(L, _detector_file_name, "Creating Dominoes Lua Module: ERROR: Unable to seek lua detector file");
    }

    validator_buffer_len = ftell(file);

    if (validator_buffer_len == -1){
        fclose(file);
        LOG_LUA_ERROR_WITH_FILENAME_AND_EXIT(L, _detector_file_name,"Creating Dominoes Lua Module: ERROR: Unable to return offset on lua detector file");	        
    }
    if (fseek(file, 0, SEEK_SET)){
        fclose(file);
        LOG_LUA_ERROR_WITH_FILENAME_AND_EXIT(L, _detector_file_name,"Creating Dominoes Lua Module: ERROR: Unable to seek lua detector file");
    }
    if ((validator_buffer = (uint8_t *) malloc(validator_buffer_len + 1)) == NULL){
        fclose(file);
        LOG_LUA_ERROR_WITH_FILENAME_AND_EXIT(L, _detector_file_name,"Creating Dominoes Lua Module: ERROR: Failed to allocate the user lua detector file");
    }
    fread(validator_buffer, validator_buffer_len, 1, file);
    validator_buffer[validator_buffer_len] = 0;

    /** compute the MD5 hash of the detector file */
    MD5INIT(&context);
    MD5UPDATE(&context, validator_buffer, validator_buffer_len);
    MD5FINAL(digest, &context);

    set_lua_state(_config);
    if (L == NULL)
    {
        free(validator_buffer);
        LOG_LUA_ERROR_WITH_FILENAME_AND_EXIT(L, _detector_file_name,"Creating Dominoes Lua Module: ERROR:can not create new lua state");
    }
    /*Load function works the same with Lua and Luac file. */
    if (luaL_loadbuffer(L, (char*)validator_buffer, validator_buffer_len, "") || lua_pcall(L, 0, 0, 0))
    {
        lua_pop(L, -1);
        lua_close(L);
        free(validator_buffer);
        LOG_LUA_ERROR_WITH_FILENAME_AND_EXIT(L, _detector_file_name, "Creating Dominoes Lua Module: ERROR:cannot run validator");
    }
    set_detector_package_info();
    set_detector_observation_list();  
    //set_detector_metadata();	
}



// Create the Lua State and load all the libraries

void  Detector::set_lua_state(DominoesModuleConfig* pc)
{

    /*Design: Whether to make all detectors share the lua_State or not?
     * Separate lua_State creates isolated environment for each detector. There
     * is no name collision or any side effects due to garbage collection. Runtime
     * memory overhead should be bearable.
     *
     * Shared lua_State saves on memory since each Lua library is loaded just once.
     */
    L = lua_open();   /* opens Lua */
    luaL_openlibs(L);

#ifdef HAVE_LIBLUAJIT
    /*linked in during compilation */
    luaopen_jit(L);

    {
        static unsigned once = 0;
        if (!once) {
            lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
            lua_getfield(L, -1, "jit");  /* Get jit.* module table. */
            lua_getfield (L, -1, "version");
            if (lua_isstring(L, -1)) {
                _dpd.logMsg("LuaJIT: Version %s\n", lua_tostring(L, -1));
            }
            lua_pop(L, 1);
            once = 1;
        }
    }

    std::cout << "LUA Stack top 1" << lua_gettop(L) << std::endl;
#endif  /*HAVE_LIBLUAJIT */

    register_detector_apis(L);
    lua_pop(L, 1); /*After detector register the methods are still on the stack, remove them. */

    register_item_apis(L);
    lua_pop(L, 1);
    //Push APIs to create and access an Item 
    //register_rollup_item_wrapper(L);
    //lua_pop(L, 1);    

    //  DetectorFlow_register(L);
    //  lua_pop(L, 1);

#if 0
#ifdef LUA_DETECTOR_DEBUG
    lua_sethook(L,&luaErrorHandler,LUA_MASKCALL | LUA_MASKRET,0);
#endif
#endif

    lua_gc(L, LUA_GCSETPAUSE,100);
    lua_gc(L, LUA_GCSETSTEPMUL,200);

    // set lua library paths 
    {
        char newLuaPath[PATH_MAX];
        lua_getglobal( L, "package" );
        lua_getfield( L, -1, "path" );
        const char * curLuaPath = lua_tostring(L, -1);

        const char* dominoesLuaLibsPath = pc->dominoes_detector_libs_path.c_str();
        printf ("\n%s\n", curLuaPath);
        printf ("\n%s\n", dominoesLuaLibsPath);
        if (curLuaPath && (strlen(curLuaPath))) {
            snprintf(newLuaPath, PATH_MAX-1,"%s%s/?.lua",
                    curLuaPath,
                    dominoesLuaLibsPath
                    );
        } else {
            // snprintf(newLuaPath, PATH_MAX-1, "%s/dominoes/libs/?.lua;%s/custom/libs/?.lua",
            snprintf(newLuaPath, PATH_MAX-1, "%s%s/?.lua",
                    curLuaPath,
                    dominoesLuaLibsPath
                    );
        }
        std::cout << newLuaPath << std::endl;
        lua_pop( L, 1 );
        lua_pushstring( L, newLuaPath);
        lua_setfield( L, -2, "path" );
        lua_pop( L, 1 );
    }

}



/**reads package_info defined inside lua detector.
*/
void Detector::set_detector_package_info() {

    //  DetectorPackageInfo  *pkg = &detector->package_info;
    LUA_GET_TABLE_START(L, "DetectorPackageInfo", true, nullptr)

    // Get all the variables 
    LUA_GET_FIELD_STRING(L, "name", package_info,detector_name, false, "")
    LUA_GET_FIELD_STRING(L, "init", package_info,init_func, false, "")
    LUA_GET_FIELD_STRING_VECTOR(L, "observation_list_names", package_info, observation_list_names, true, "default_observation" )
     
    // Get the tick_callback_function that is called after each tick

    lua_getfield(L, -1, "timed_callbacks"); //table
    if (!lua_istable(L, -1)) {
        //TODO: add a default selector
        Log("The callback function has to provide a callback function name and duration");
        exit (-1);
    }	    
    
    for (int i=1; i<= luaL_getn(L, -1); i++) {
	Detector_Timed_Callback *dtc = new Detector_Timed_Callback;	
	try {
		lua_rawgeti(L,-1,i);
		if (lua_istable(L, -1)) {
			lua_getfield_(L, "name", dtc->name); // string
			lua_getfield_(L, "duration", dtc->duration); // string 
		} else{
			throw bad_field_type("table");
		}
		
		
    	} catch(...) {
        	Log("syntax: tick_callback_fn = {name (string), duration(number)}");
        	exit(-1);
    	}
   	 
   	dtc->L = L;
    	dtc->count_duration = 0;
    	package_info.tick_callback_fn_list.push_back(*dtc);
    	delete dtc;
	lua_pop(L,1);
    }

    lua_pop(L,1);


    // Get the metadata source configuration
/*
    lua_getfield(L, -1, "metadata_source_conf"); //table
    if (!lua_istable(L, -1)) {
        //TODO: add a default selector
        Log("The metadata source configuration configures the metadata source that will be used by the script");
        exit (-1);
    }

   Metadata_Source_Config *msc = new Metadata_Source_Config;
   try {
        lua_getfield_(L, "name", msc->metadata_name); // string 
        lua_getfield_(L, "source", msc->metadata_source); // string 
        lua_getfield_(L, "source_type", msc->metadata_source_type); // string  
   } catch(...) {
        Log("syntax: metadata_source_conf = {metadata_type (string), source(string), source_type (string)}");
        exit(-1);
   }
   package_info.metadata_source_config.push_back(*msc);	
   lua_pop(L, 1);	 	
*/
  // Pop the DetectorPackageInfo table  
  lua_pop(L, 1);

}

/**reads package_info defined inside lua detector.
*/
void Detector::set_detector_observation_list ()
{
   if (package_info.observation_list_names.size()) {
        for (auto &list_iter : package_info.observation_list_names) {
            std::shared_ptr<Observation> observation(new Observation);
            observation->fill_configuration(L, list_iter,this);


            /* Write a snort rule to the dominoes rules file */
            add_observation_to_dominoes_rules_file(observation);

            /* Add observation to observation list*/
            observation_list.push_back(observation);
        }  // end of for loop
    } else {
        std::cout << "Instantiate a default selector for: " <<  get_detector_name() << std::endl;
    }
}

//TODO: This will be replaced by a in-memory data store later
/* load the metadata */
void Detector::set_detector_metadata(){
	for(auto &metadata_config: package_info.metadata_source_config){
		Metadata_Object* mo = new Metadata_Object();
		mo->set_name(metadata_config.metadata_name);
		// TODO Error checks
		if (metadata_config.metadata_source_type == "file"){
			std::cout << "Set Detector Metadata" << metadata_config.metadata_source << std::endl;
			std::string key,val;
			std::ifstream metdata_file;
			metdata_file.open(metadata_config.metadata_source, std::ios::in);
			while(metdata_file >> key >> val){
			//	std::cout << "(" << key << ", " << val << ")" << std::endl;
			//	uint64_t key_index = mo->get_key_dictionary().insert(key);
			//	uint64_t val_index = mo->get_val_dictionary().insert(val);
				mo->insert(key, val);
			} 	
		}
		mao.insert(*mo);	
	}
}

/* Write a snort rule to the dominoes rules file */
void Detector::add_observation_to_dominoes_rules_file(std::shared_ptr<Observation> _observation)
{
   std::ofstream dominoes_rules_file;
   //TODO: hard-coded location of Dominoes rules file to be removed
   // This is Dominoes level, move ot from here
    dominoes_rules_file.open("/tmp/dominoes.rules", std::ios::out | std::ios::app);
    if (!(dominoes_rules_file.is_open())) {
        printf ("\nCould not open dominoes signature file for writing\n");
        exit(-1);
    } else {
        printf("\nOpened dominoes rules file \n");
    }
    
 
    /* Write a snort rule to the dominoes rules file */
    dominoes_rules_file << "dominoes_alert "
        << _observation->get_event_filter().get_ev_rule_header()
        << "("
        << _observation->get_event_filter().get_ev_rule_options()
        << "sid:"
        << static_cast<std::ostringstream*>( &(std::ostringstream() 
                    << _observation->get_event_filter().get_ev_rule_local_sid() ) )->str() << ";"
        << ")"
        << std::endl;
}

void Detector::call_detector_init_func(){
	if (package_info.init_func.size()){
		lua_getglobal(L, package_info.init_func.c_str());
	        std::cout << "call detecor Init detector_user_data_ref: " << detector_user_data_ref << std::endl; 
         	lua_pushnumber(L, detector_user_data_ref);
		assert(lua_isfunction(L, -1));
		if (lua_pcall(L, 1, 0, 0) != 0) {
         	   printf("Error running function: %s`", lua_tostring(L, -1));
        	}
	}
}



/** Dominoes Detector Timed Callback */
Detector_Timed_Callback::Detector_Timed_Callback( char* _name, uint32_t _duration,lua_State *_L) :
    name(_name),
    L(_L)
{
    struct timespec t = {_duration,0};
    //TODO: CALCULATE NUMBER OF TICKS
    count_duration = 5;

}

void Detector_Timed_Callback::call_detector_timed_callback_fn(std::shared_ptr<Detector> _detector) {
    if (count_duration > 1 ) {
        count_duration--;
        return;
    } else {
        count_duration = duration;
        lua_getglobal(L, name.c_str());
        // Push the reference to the detector user data as a parameter to the lua function
        lua_pushnumber(L, _detector->get_detector_user_data_ref());
        assert(lua_isfunction(L, -1));

        if (lua_pcall(L, 1, 0, 0) != 0) {
            printf("Error running function :%s", lua_tostring(L, -1));
        }

    }
}



