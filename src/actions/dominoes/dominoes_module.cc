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

// dominoes_module.cc author Subharthi Paul <subharpa@cisco.com>

#include "dominoes_module.h"

#include <assert.h>
#include <sstream>

#include "utils/util.h"

using namespace std;

#define default_detector_scripts_path  "/usr/local/lib/temp/dominoes/applets"
#define default_detector_libs_path "/usr/local/lib/temp/dominoes/libs"
#define default_dominoes_generated_snort_rules_file "/usr/local/snort3/etc/snort/dominoes.rules"

static const Parameter s_params[] =
{
	{ "dominoes_detector_scripts_path", Parameter::PT_STRING, nullptr, default_detector_scripts_path,
		"path to the directory where the Dominoes Detector Lua Scripts are placed " },

	{ "dominoes_detector_libs_path", Parameter::PT_STRING, nullptr, default_detector_libs_path,
		"path to the directory where the Dominoes Detector Library Scripts are placed " },

	{"dominoes_generated_snort_rules_file", Parameter::PT_STRING, nullptr, default_dominoes_generated_snort_rules_file,
		"path to the dominoes generated snort rules file"},

	{ nullptr, Parameter::PT_MAX, nullptr, nullptr, nullptr }
};

//-------------------------------------------------------------------------
// dominoes module
//-------------------------------------------------------------------------

DominoesModule::DominoesModule() : Module(DOMINOES_NAME, DOMINOES_HELP, s_params)
{
	printf ("\nInside Dominoes Module constructor\n");
	conf = nullptr;
}

DominoesModule::~DominoesModule()
{
	if ( conf )
		delete conf;
}


bool DominoesModule::set(const char*, Value& v, SnortConfig*)
{

	if ( v.is("dominoes_detector_scripts_path") ){
		conf->dominoes_detector_scripts_path = v.get_string();
		printf ("\nDominoes module: detector scripts path: %s\n", conf->dominoes_detector_scripts_path.c_str());
		return true;			
	}
	if ( v.is("dominoes_detector_libs_path")){
		conf->dominoes_detector_libs_path = v.get_string();
		printf ("\nDominoes module: detector libs path: %s\n", conf->dominoes_detector_libs_path.c_str());
		return true;
	}
	if (v.is("dominoes_generated_snort_rules_file")){
		conf->dominoes_generated_snort_rules_file =  v.get_string();
		return true;
	}	

	   	
	return false; 
}

DominoesModuleConfig* DominoesModule::get_conf_data()
{
	DominoesModuleConfig* tmp = conf;
//	conf = nullptr;
	return tmp;

}
bool DominoesModule::begin(const char*, int, SnortConfig*)
{
	conf = new DominoesModuleConfig;
	
	//conf->dominoes_detector_scripts_path = default_detector_scripts_path;
	//conf->dominoes_detector_libs_path = default_detector_libs_path;

	return true;
}



