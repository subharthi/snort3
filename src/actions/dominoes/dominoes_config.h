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
** Project Dominoes <Subharthi Paul:subharpa@cisco.com>
**  
** Ported from the OpenAppID implementation in Snort and modified for Project Dominoes
*/


#ifndef __DOMINOES_CONFIG_H___
#define __DOMINOES_CONFIG_H___

#include <stdint.h>
#include <limits.h>
#include <string>
#include <strings.h>

struct DominoesModuleConfig 
{
        std::string dominoes_detector_scripts_path;
	std::string dominoes_detector_libs_path;
	std::string dominoes_generated_snort_rules_file;
};

/*
class DominoesConfig
{
public:
	void dominoes_config_parse(char *args);
 	void dominoes_config_dump(void);
        static DominoesConfig* init();
private:
	DominoesConfig();
	char dominoes_detector_path[PATH_MAX];
        static DominoesConfig *m_instance; 

};
*/
//extern struct DominoesCommandConfig *dominoesCommandConfig;
//void dominoesConfigParse(char *args);
//void dominoesConfigDump(void);
#endif
