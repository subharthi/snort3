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
//

// dominoes_globals.h Author: Subharthi Paul <subharpa@cisco.com>

#ifndef DOMINOES_GLOBALS_HPP
#define DOMINOES_GLOBALS_HPP

#include "lua/lua.h"
#include "lua/lua_util.h"
#include <vector>
#include <memory>
#include "dominoes_detector.h"
#include "observation.h"
#include "accumulators/src/statistics.hpp"
#include "accumulators/extractors/real_extractors.hpp"
#include "accumulators/src/accumulator_group.hpp"
#include <iostream>

class Observation;
class Detector;

#define MAX_OBSERVATIONS 1000
#define SID_START_INDEX 100000


/* using thread_local for dynamic instantiation */
extern  thread_local std::vector <std::shared_ptr<Detector> > detector_list;
extern  thread_local uint32_t event_local_sid; 

/* common error logging function */ 
void LOG_LUA_ERROR_WITH_FILENAME_AND_EXIT(lua_State *L, std::string _file_name, std::string _message);
void LOG_LUA_ERROR_AND_EXIT(lua_State *L,std::string _message);

void LOG_ERROR_WITH_FILENAME_AND_EXIT(std::string _file_name, std::string _message);
void LOG_ERROR_AND_EXIT(std::string _message);
// TODO
// This will eventually go away
static const extractors::packet::ipv4::extractor &get_extractor(int e)
{
   // will call the constructor of packet_data_container(packet_data_args)
    using namespace extractors::packet;
    switch(static_cast<ipv4::Field>(e)) {
        case ipv4::Field::hlen:     return ipv4::hlen;
        case ipv4::Field::ver:      return ipv4::ver;
        case ipv4::Field::tos:      return ipv4::tos;
        case ipv4::Field::len:      return ipv4::len;
        case ipv4::Field::ttl:      return ipv4::ttl;
        case ipv4::Field::proto:    return ipv4::proto;
        case ipv4::Field::src_ip:   return ipv4::src_ip;
        case ipv4::Field::dst_ip:   return ipv4::dst_ip;
	case ipv4::Field::client_ip: return ipv4::client_ip;
	case ipv4::Field::server_ip: return ipv4::server_ip;
	case ipv4::Field::client_port: return ipv4::client_port;
	case ipv4::Field::server_port: return ipv4::server_port;	
        default:                    throw("Invalid key");
    }
}

#endif
