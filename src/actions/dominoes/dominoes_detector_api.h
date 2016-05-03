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

#ifndef _DOMINOES_DETECTOR_API_H_
#define  _DOMINOES_DETECTOR_API_H_

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
#include "dominoes_detector.h"
#include "dominoes_globals.h"
#include "rollup/src/rollup.h"
#include "lua_rollup_item_wrapper.h"
#define DETECTOR_API "detector_api"
#define QUERY_API "query_api"
#define OUTPUT_API "output_api"

extern const struct timespec dominoes_tick;

class Detector;
/**data directly accessed by Lua code should be here.
*/
struct DetectorUserData {
	/**points to detector allocated on the C side. This is needed to get detector from callback functions. This pointer must be set to
	 * NULL when Detector is destroyed.
	 */
	std::shared_ptr<Detector> pDetector;

	/*lua accessible data elements should be defined below. */
};

// Create an instance of Item that is returned for al queries to the Rollup data
/*struct ItemWrapper{
        Item* item;
        DetectorUserData* detector_ref; 
	std::string observation_name; 
}; 
*/
int register_detector_apis (lua_State *L);
int register_item_apis (lua_State *L);
DetectorUserData *check_detector_user_data (lua_State *L,int index);

DetectorUserData *pushDetectorUserData(lua_State *L, std::shared_ptr<Detector> _detector);

//extern thread_local std::vector <Detector> detector_list ;
#endif

