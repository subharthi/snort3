//--------------------------------------------------------------------------
// Copyright (C) 2014-2016 Cisco and/or its affiliates. All rights reserved.
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
// action_manager.cc author Russ Combs <rucombs@cisco.com>

#include "action_manager.h"

#include <assert.h>

#include <list>
using namespace std;

#include "main/snort_types.h"
#include "main/snort_config.h"
#include "main/snort_debug.h"
#include "utils/util.h"
#include "managers/module_manager.h"
#include "framework/ips_action.h"
#include "parser/parser.h"
#include "log/messages.h"
#include "actions/act_replace.h"
#include "packet_io/active.h"

struct Actor
{
    const ActionApi* api;
    IpsAction* act;

    Actor(const ActionApi* p)
    { api = p; act = nullptr; }
};

typedef list<Actor> AList;
static AList s_actors;

static IpsAction* s_reject = nullptr;
static THREAD_LOCAL IpsAction* s_action = nullptr;

/* Extended for Project Dominoes: Subharthi Paul <subharpa@cisco.com> */
IpsAction* s_action_context = nullptr;
static thread_local std::list<const OptTreeNode*> s_otn;

//-------------------------------------------------------------------------
// action plugins
//-------------------------------------------------------------------------

void ActionManager::add_plugin(const ActionApi* api)
{
    Actor a(api);

    if ( api->pinit )
        api->pinit();

    s_actors.push_back(a);
}

void ActionManager::release_plugins()
{
    for ( auto& p : s_actors )
    {
        p.api->dtor(p.act);

        if ( p.api->pterm )
            p.api->pterm();
    }
    s_actors.clear();
}

void ActionManager::dump_plugins()
{
    Dumper d("IPS Actions");

    for ( auto& p : s_actors )
        d.dump(p.api->base.name, p.api->base.version);
}

static void store(const ActionApi* api, IpsAction* act)
{
    for ( auto& p : s_actors )
        if ( p.api == api )
        {
            //assert(!p.act);  FIXIT-H memory leak on reload; move to SnortConfig?
            p.act = act;
            break;
        }
}

//-------------------------------------------------------------------------

RuleType ActionManager::get_action_type(const char* s)
{
    for ( auto& p : s_actors )
    {
        if ( !strcmp(p.api->base.name, s) )
            return p.api->type;
    }
    return RULE_TYPE__NONE;
}

void ActionManager::instantiate(
    const ActionApi* api, Module* m, SnortConfig* sc)
{
    IpsAction* act = api->ctor(m);

    if ( act )
    {
        if ( !s_reject && !strcmp(act->get_name(), "reject") )
            s_reject = act;

        ListHead* lh = CreateRuleType(sc, api->base.name, api->type);
        assert(lh);
        lh->action = act;

        store(api, act);
    }
}

void ActionManager::thread_init(SnortConfig*)
{
    for ( auto& p : s_actors )
        if ( p.api->tinit )
            p.api->tinit();
}

void ActionManager::thread_term(SnortConfig*)
{
    for ( auto& p : s_actors )
        if ( p.api->tterm )
            p.api->tterm();
}

/* Extended for Project Dominoes: Subharthi Paul <subharpa@cisco.com> 
*  Check if s_action_context is not null indiating pending dominoes_alert
*  Since the execute function is called once per packet, we record the events generated in a list 
*  and call the exec_context function once for each event
*/
void ActionManager::execute(Packet* p)
{
   if (s_action_context){
                for (auto& otn_node : s_otn){
                        s_action_context->exec_context(p,otn_node);
                }
                s_otn.clear();
                s_action_context = nullptr;
   } else 
   if ( s_action )
    {
        s_action->exec(p);
        s_action = nullptr;
    }
}

/* Project Dominoes: Subharthi Paul <subharpa@cisco.com> 
*
* Extending the 
* Previously:  ActionManager::queue(IpsAction* a)
* Now: ActionManager::queue(IpsAction* a, RuleTreeNode* rtn, OptTreeNode* otn)
* This is to propagate the rule gid and sid to the IPSAction Module
* Note, we are storing the otn in a list 
*/
void ActionManager::queue(IpsAction* a, const RuleTreeNode* rtn, const OptTreeNode* otn)
{
   if (!strcmp(a->get_name(),"dominoes_alert"))
   {
                s_action_context = a;
                s_otn.push_back(otn);
   } else 
   if ( !s_action || a->get_action() > s_action->get_action() )
        s_action = a;
}

void ActionManager::queue_reject(const Packet* p)
{
    if ( !p->ptrs.ip_api.is_ip() )
        return;

    switch ( p->type() )
    {
    case PktType::TCP:
        if ( !Active::is_reset_candidate(p) )
            return;
        break;

    case PktType::UDP:
    case PktType::ICMP:
    case PktType::IP:
        if ( !Active::is_unreachable_candidate(p) )
            return;
        break;

    default:
        return;
    }
    if ( s_reject )
	/* Project Dominoes: Subharthi Paul <subharpa@cisco.com> */
        /* Note: the signature of the queue function has changed */
        queue(s_reject, nullptr, nullptr);
}

void ActionManager::reset_queue()
{
    s_action = nullptr;

    /* Extended for Project Dominoes Subharthi Paul <subharpa@cisco.com> */
    s_action_context = nullptr;
    s_otn.clear();

    Replace_ResetQueue();
}

#ifdef PIGLET

//-------------------------------------------------------------------------
// piglet breach
//-------------------------------------------------------------------------

static const ActionApi* find_api(const char* name)
{
    for ( auto actor : s_actors )
        if ( !strcmp(actor.api->base.name, name) )
            return actor.api;

    return nullptr;
}

IpsActionWrapper* ActionManager::instantiate(const char* name, Module* m)
{
    auto api = find_api(name);
    if ( !api || !api->ctor )
        return nullptr;

    auto p = api->ctor(m);
    if ( !p )
        return nullptr;

    return new IpsActionWrapper(api, p);
}

#endif

