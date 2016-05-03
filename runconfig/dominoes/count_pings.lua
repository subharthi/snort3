--[[
# Copyright 2001-2014 Cisco Systems, Inc. and/or its affiliates. All rights
# reserved.
#
# This file contains proprietary Detector Content created by Cisco Systems,
# Inc. or its affiliates ("Cisco") and is distributed under the GNU General
# Public License, v2 (the "GPL").  This file may also include Detector Content
# contributed by third parties. Third party contributors are identified in the
# "authors" file.  The Detector Content created by Cisco is owned by, and
# remains the property of, Cisco.  Detector Content from third party
# contributors is owned by, and remains the property of, such third parties and
# is distributed under the GPL.  The term "Detector Content" means specifically
# formulated patterns and logic to identify applications based on network
# traffic characteristics, comprised of instructions in source code or object
# code form (including the structure, sequence, organization, and syntax
# thereof), and all documentation related thereto that have been officially
# approved by Cisco.  Modifications are considered part of the Detector
# Content.
--]]

-- import the types
require "types"


local types = types

-- configure package parameters
DetectorPackageInfo = {
    name =  'ping-counter',
    observation_list_names = { 'observation_ping_counts'},
    tick_callback_fn_list = { name = "tick_callback_fn", duration = 1}	
}

-- configure observation events 
observation_ping_counts = { name = 'ping-counts', 
		   	    event_filter_rule_header = "icmp any any -> any any", event_filter_rule_options = " ", event_filter_rule_global_sid = "sid:14444; rev:1",
			    statistics_list = { -- list of statistcs for this observation
					         { name = 'count_icmp', 
					           key = {types.key_value_type.ipv4.src_ip},
					           -- key = {types.key_value_type.ipv4.src_ip}, 
				                  value = types.key_value_type.ipv4.src_ip, 
					          accumulator_set = {types.metric.COUNT}
					         }
			                      }
                          }


function tick_callback_fn(detector_ref)
	-- detector_api is the c library loaded into the scripts environment by dominoes 
	-- detector_ref is required to invoke the c api's
	-- detector_api.print_stats(detector_ref);
	print ("Tick callback function");
	detector_api.get_stats(detector_ref, "ping-counts", "count_icmp");
	--detector_api.get_stats(detector_ref,"syn-ack-counts","count_half_open_conn_per_src_ip");
end


-- Functions for 'init', 'clean', and 'process'
function event_handler_init (detectorInstance)

  -- initialize the handler

end

function event_handler_clean ()

 -- destroy the handler
end

function event_handler_process ()


   -- handle events

end

function anomaly_handler_init (detectorInstance)

 -- initialize anomaly handler	

end

function anomaly_handler_clean()

 -- destroy handler 

end


function anomaly_handler_process()


 -- process a anomaly trigger

end


