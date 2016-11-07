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


// timer_thread.cc author Subharthi Paul <subharpa@cisco.com>
// timer_thread

#include "dominoes_timer_thread.h"
#include "dominoes_detector_api.h"
#include <time.h>

const struct timespec dominoes_tick = {10,0}; // 3 seconds 


//TimerCallback
void TimerCallbacks::operator()(unsigned _idx, const std::vector <std::shared_ptr<Detector> > &_detector_list_t )
{
	idx = _idx;
	detector_list_t = _detector_list_t;
        // TODO: Do we need time
	time_t st = time(NULL);
	
	while (runflag) {
		nanosleep(&dominoes_tick, NULL);
		time_t ft = time(NULL);
		if (detector_list_t.size()) { 
			for (auto& iter_detector_list: detector_list_t) {
				// TODO: Rollup at each tick and let the script query the rolled up data structure
				// TODO: This will lead to a cascade of rollups
				// TODO: this needs to be in a separate function
                        	for(auto& iter_observation_list:  iter_detector_list->observation_list) {
                               		 // time_t ft = time(NULL);
					accumulator_table_map_type table = iter_observation_list->get_statistics().dump();
					Item *itm = new Item(st,ft,table);
					iter_observation_list->get_rollup().addItem(itm);
					//ADDED by staghavi@cisco, static call to export function per tick
					//TODO: staghavi@cisco.com, Change where and how export function get called
					Export *export_item = new Export(iter_detector_list, "tick.json");
					//TODO: staghavi@cisco.com, Overload print_json_point without passing any feature to export
					// all available features instead of only one!
					metric::metric_type feature = static_cast<metric::metric_type>(3);
					export_item->print_json_point(feature);
				}					
	
				for (auto& callback : iter_detector_list->get_detector_package_info().tick_callback_fn_list) {
						callback.call_detector_timed_callback_fn(iter_detector_list);
                		}
			}
		} else {
            		std::cout << "Timer callback wokeup but detector list is null" << std::endl ;
        	}
	       st = ft;    
	}
}



//TimerThread

void TimerThread::start(unsigned _idx, const std::vector <std::shared_ptr<Detector> > &_detector_list_t )
{
    std::cout << "Starting timer thread " << _idx << std::endl; 
    timer_callbacks = new TimerCallbacks();
    timer_callbacks->set_runflag(true);

    t_thread = new std::thread(std::ref(*timer_callbacks), _idx, std::ref(_detector_list_t));

}

void TimerThread::stop(unsigned _idx)
{
    timer_callbacks->set_runflag(false);
    t_thread ->join();
    delete t_thread;
    std::cout << "killed Timer thread" << std::endl;
}
