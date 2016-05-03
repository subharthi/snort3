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

// dominoes.h Author: Subharthi Paul <subharpa@cisco.com>

#ifndef DOMINOES_H
#define DOMINOES_H
// Implementation header with definitions, datatypes and flowdata class for SIP service inspector.

#include <memory>

#include "main/snort_types.h"
#include "framework/ips_action.h"

#include "protocols/packet.h"
#include "stream/stream_api.h"
#include "dominoes_config.h"
#include "dominoes_module.h"
#include "observation.h"
#include "framework/data_bus.h"
/*
*  Dominoes class 
*/

class Dominoes : public IpsAction
{
    public:
    Dominoes(DominoesModuleConfig* config);

    void exec_context(const Packet *p, const OptTreeNode *otn ) override;

    /* 
     * exec is defined as pure virtual function in IpsAction; so need to implement it
     * exec is not used for the dominoes IPS module
     * instead; the exec_context() method is called for dominoes
     */
    void exec(Packet* p) override;

  
    private:

    /* helper functions */
    void load_dominoes_detectors(DominoesModuleConfig* config);
    void load_observation_map();

    /** Map of observation of exec_context to send packets */
    std::vector<std::shared_ptr<Observation> > observation_map;

    /** snort config */
    DominoesModuleConfig* config;	

};


#endif
