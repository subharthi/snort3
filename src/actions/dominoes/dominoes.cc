//--------------------------------------------------------------------------
// Copyright (C) 2015-2015 Cisco and/or its affiliates. All rights reserved.
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


// dominoes.cc author Subharthi Paul <subharpa@cisco.com>
// Dominoes Module

#include "dominoes.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
#include <thread>
#include "main/snort_types.h"
#include "main/snort_debug.h"
#include "main/snort_config.h"
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

#include "dominoes_detector_api.h"
#include "dominoes_module.h"
#include "hash/sfxhash.h"
#include "utils/sflsq.h"
#include "detection/treenodes.h"
#include "accumulators/src/statistics.hpp"
#include "accumulators/extractors/real_extractors.hpp"
#include "dominoes_module.h"
#include "dominoes_macros.h"
#include "dominoes_detector.h"
#include "dominoes_globals.h"

#define DETECTOR "Detector"
#define GET_SRC_IP(p) ((p)->ptrs.ip_api.get_src())
#define GET_DST_IP(p) ((p)->ptrs.ip_api.get_dst())


class EventHandler: public DataHandler
{

public:
    EventHandler(std::string s)
    { key = s; }

    void handle(DataEvent& e, Flow*);

private:
    std::string key;
};

void EventHandler::handle(DataEvent& e, Flow* f)
{
    unsigned n;
    const char* b = (char*)e.get_data(n);
    std::cout << "Key: " << key << " hostname: " << (std::string(b)).substr(0,n) << std::endl;
   // std::cout << " URI Handler" << std::endl;
}


Dominoes::Dominoes(DominoesModuleConfig* _config):
    IpsAction(DOMINOES_NAME, ACT_PROXY),
    observation_map(MAX_OBSERVATIONS),
    config(_config)
{
	std::ofstream dominoes_rules_file;
	//TODO: hard-coded location of Dominoes rules file to be removed
   	// This is Dominoes level, move ot from here
    	dominoes_rules_file.open("/tmp/dominoes.rules", std::ios::out | std::ios::trunc);
    	if (!(dominoes_rules_file.is_open())) {
        	printf ("\nCould not open dominoes signature file for writing\n");
        	exit(-1);
    	} else {
       	 	std::cout << "truncated the rules file" << std::endl;
    	}
	dominoes_rules_file.close();

	/* load the dominoes lua detectors */
	std::cout << "Creating the Dominoes IPSAction Object: " <<  config->dominoes_detector_scripts_path << std::endl;
	load_dominoes_detectors(config);
	load_observation_map();
	//get_data_bus().subscribe("http_hostname", new EventHandler("http_uri"));	
	std::cout << "Dominoes configuration thread id :" << std::this_thread::get_id() << std::endl;	
}


void Dominoes::exec(Packet* p)
{
    /* 
     * exec is defined as pure virtual function; so need to implement it
     * exec is not used for the dominoes IPS module
     * instead; the exec_context() method is called for dominoes
     */
}

void Dominoes::exec_context(const Packet *p, const OptTreeNode *otn)
{
    uint32_t index = otn->sigInfo.id - SID_START_INDEX;
    
    /** send packet for processing */
    observation_map[index]->get_statistics()(*p);
}



// Loading Dominoes Detectors
void Dominoes::load_dominoes_detectors(DominoesModuleConfig* _config)
{

    std::cout << "Creating Dominoes Lua Module: Loading Scripts " <<  _config->dominoes_detector_scripts_path << std::endl;

    int rval;
    glob_t globs;

    std::string pattern = config->dominoes_detector_scripts_path + "/*.lua";
    memset(&globs, 0, sizeof(globs));
    rval = glob(pattern.c_str(), 0, NULL, &globs);
    if (rval != 0 && rval != GLOB_NOMATCH)
    {
        LOG_ERROR_AND_EXIT(std::string("Creating Dominoes Lua Module: ERROR: Unable to read directory: ") + pattern);
    }

    for (uint32_t n = 0; n < globs.gl_pathc; n++)
    {
        std::shared_ptr<Detector> detector(new Detector) ; 
        detector->load_detector(_config, std::string(globs.gl_pathv[n]));
        
	/* Push pUserData to the stack */
        DetectorUserData *pUserData = pushDetectorUserData(detector->get_lua_state(), detector);
        
	/* pop stack and save pUserData in the registry and return the ikey in the registry */  
        detector->set_detector_user_data_ref(luaL_ref( detector->get_lua_state(), LUA_REGISTRYINDEX));
        std::cout << "Detector user data: " << detector->get_detector_user_data_ref() << std::endl;
	detector->call_detector_init_func();
        detector_list.push_back(detector);
    }
    globfree(&globs);
}


void Dominoes::load_observation_map(){

    /** detector list has thread local storage shared with the timer thread */
    /** observation_map is created and stored by the control thread for use in the data thread */
    //TODO: is there a better way to share
    for(auto& detector_iter_obj : detector_list){
        for (auto& observation_iter_obj: detector_iter_obj->get_observation_list()){
            if (observation_iter_obj->get_event_filter().get_ev_rule_local_sid()  - SID_START_INDEX > MAX_OBSERVATIONS){
                LOG_ERROR_AND_EXIT(std::string("Number of observations registered exceeded the Max Limit:") + std::to_string(MAX_OBSERVATIONS));
            }
            observation_map[(observation_iter_obj->get_event_filter().get_ev_rule_local_sid()  - SID_START_INDEX)] = observation_iter_obj;
        }
    }
}

//-------------------------------------------------------------------------
// api stuff
//-------------------------------------------------------------------------

static Module* mod_ctor()
{ 
    printf("\nCreating Dominoes Module: Constructor");
    return new DominoesModule; 
}

static void mod_dtor(Module* m)
{ 
    delete m;
}

static void dominoes_init()
{
    printf("\nCreating Dominoes Module: Dominoes init \n");
}

static IpsAction* dominoes_ctor(Module* m)
{
    DominoesModule* mod = (DominoesModule*)m;
    return new Dominoes(mod->get_conf_data());
}

static void dominoes_dtor(IpsAction* p)
{
    delete p;
}



const ActionApi dominoes_api =
{
    {
        PT_IPS_ACTION,
        sizeof(ActionApi),
        ACTAPI_VERSION,
        0,
        API_RESERVED,
        API_OPTIONS,
        DOMINOES_NAME,
        DOMINOES_HELP,
        mod_ctor,
        mod_dtor
    },
    RULE_TYPE__ALERT,
    dominoes_init, //pinit
    nullptr, // pterm
    nullptr, // tinit
    nullptr, // tterm
    dominoes_ctor,
    dominoes_dtor,
};

#ifdef BUILDING_SO
SO_PUBLIC const BaseApi* snort_plugins[] =
{
    &dominoes_api.base,
    nullptr
};
#else
const BaseApi* act_dominoes = &dominoes_api.base;
#endif

