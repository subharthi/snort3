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


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>

//#include "sf_dynamic_preprocessor.h"
#include "dominoes_config.h"
//#include "common_util.h"


#define DEFAULT_DOMINOES_DETECTOR_PATH "/usr/local/etc/dominoes"

/*
static DominoesCommandConfig *DominoesCommandConfig::init(){

	if(m_instance == NULL){
	 	m_instance = new DominoesCommandConfig();
 		return m_instance;
	} else
		return m_instance;
}


void DominoesCommandConfig::dominoesConfigParse(char *args)
{
    char **toks;
    int num_toks;
    int i;
    char **stoks;
    int s_toks;
    char *endPtr;

    if ((args == NULL) || (strlen(args) == 0))
        return;

    appIdCommandConfig = &config;

    memset (appIdCommandConfig, 0, sizeof(*appIdCommandConfig));

    toks = _dpd.tokenSplit(args, ",", 0, &num_toks, 0);
    i = 0;

    for (i = 0; i < num_toks; i++)
    {
        stoks = _dpd.tokenSplit(toks[i], " ", 2, &s_toks, 0);

        if (s_toks == 0)
        {
            _dpd.fatalMsg("%s(%d) => %s\n", *(_dpd.config_file), *(_dpd.config_line), "Missing Dominoes configuration");
            exit(-1);
        }

        else if(!strcasecmp(stoks[0], "dominoes_detector_dir"))
        {
            if (!stoks[1] || strlen(stoks[1]) >= sizeof(appIdCommandConfig->app_id_detector_path))
            {
                _dpd.fatalMsg("%s(%d) => %s\n", *(_dpd.config_file), *(_dpd.config_line), "Invalid app_detector_dir");
                exit(-1);
            }

            snprintf(appIdCommandConfig->app_id_detector_path, sizeof(appIdCommandConfig->app_id_detector_path), "%s", stoks[1]);
        }
        else
        {
            DynamicPreprocessorFatalMessage("%s(%d) => Unknown dominoes configuration option \"%s\"\n",
                                            *(_dpd.config_file), *(_dpd.config_line), toks[i]);
        }

        _dpd.tokenFree(&stoks, s_toks);
    }


    if (!appIdCommandConfig->app_id_detector_path[0])
        snprintf(appIdCommandConfig->app_id_detector_path, sizeof(appIdCommandConfig->app_id_detector_path), "%s", DEFAULT_APPID_DETECTOR_PATH);

    _dpd.tokenFree(&toks, num_toks);

    appIdConfigDump();
}

void appIdConfigDump(void)
{
    _dpd.logMsg("AppId Configuration\n");

    _dpd.logMsg("    Detector Path:          %s\n", appIdCommandConfig->app_id_detector_path);
    _dpd.logMsg("    appStats Files:         %s\n", appIdCommandConfig->app_stats_filename? appIdCommandConfig->app_stats_filename:"NULL");
    _dpd.logMsg("    appStats Period:        %d secs\n", appIdCommandConfig->app_stats_period);
    _dpd.logMsg("    appStats Rollover Size: %d bytes\n", appIdCommandConfig->app_stats_rollover_size);
    _dpd.logMsg("    appStats Rollover time: %d secs\n", appIdCommandConfig->app_stats_rollover_time);
    _dpd.logMsg("\n");
}

*/
