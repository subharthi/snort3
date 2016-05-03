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


// dominoes_timer_thread.h author Subharthi Paul <subharpa@cisco.com>
// domioes_timer_thread

#ifndef DOMINOES_TIMER_THREAD_H
#define DOMINOES_TIMER_THREAD_H

// Timer Callback object
#include <vector>
#include <thread>
#include "dominoes_detector_api.h"

class TimerCallbacks
{
    public:
    TimerCallbacks(){};
    void operator()(unsigned _idx, const std::vector <std::shared_ptr<Detector> > &_detector_list_t );
    void set_runflag(bool _runflag){ runflag = _runflag; }

    private:
    std::vector <std::shared_ptr<Detector> > detector_list_t ;
    bool runflag;
    int idx;

};



//TimerThread

class TimerThread
{
    public:
    std::thread* t_thread;
    TimerCallbacks* timer_callbacks;
    TimerThread(){timer_callbacks = nullptr;};
    void start(unsigned _idx, const std::vector <std::shared_ptr<Detector> > &_detector_list_t );
    void stop (unsigned _idx);

};







#endif


