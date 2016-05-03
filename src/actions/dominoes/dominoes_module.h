//--------------------------------------------------------------------------
// Copyright (C) 2014-2015 Cisco and/or its affiliates. All rights reserved.
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

// dominoes_module.h author Subharthi Paul <subharpa@cisco.com>

#ifndef DOMINOES_MODULE_H
#define DOMINOES_MODULE_H

#include "main/snort_types.h"
#include "framework/module.h"
#include "flow/flow_control.h"
#include "dominoes_config.h"

//extern THREAD_LOCAL ProfileStats s5PerfStats;
struct SnortConfig;

//-------------------------------------------------------------------------
// dominoes module
//-------------------------------------------------------------------------

#define DOMINOES_NAME "dominoes_alert"
#define DOMINOES_HELP "inline statistical preprocessor"


class DominoesModule : public Module
{
    public:
    DominoesModule();
    ~DominoesModule();

    bool set(const char*, Value&, SnortConfig*) override;
    bool begin(const char*, int, SnortConfig*) override;
    //    bool end(const char*, int, SnortConfig*) override;

    DominoesModuleConfig* get_conf_data();


    private:
    DominoesModuleConfig* conf;
    std::string dominoes_detector_scripts_path;
};


#endif


