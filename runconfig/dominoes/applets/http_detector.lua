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
#
# Author: Subharthi Paul <subharpa@cisco.com> 
#
#
--]]

-- import the types
require "types"
require "service_configurations"
local ipv4 = types.key_value_type.ipv4
local feature = types.metric
local mapper = service_configurations 

----- configure package parameters -------
DetectorPackageInfo = {
    name =  'http-detector',
    init =  'init',
    observation_list_names = { 'http_observer' },
    timed_callbacks = { {name = 'threshold_function', duration = 1},
			{name = 'export_function', duration = 2} }
   	
}

-- configure observation events ------------ 

http_observer  = { 
    name = 'http-observation', 
    event_filter_rule_header = "tcp any any <> any any", 
--    event_filter_rule_options = "content:\"http://timesofindia.indiatimes.com/\"; ", 
 --   event_filter_rule_options = "content:\"*\"; http_method; ",
 --   event_filter_rule_options = "content:\"*\"; http_uri; ", 
  
    event_filter_rule_options = "",
    event_filter_rule_global_sid = "sid:14444; rev:1",

    statistics_list = {    
          -- list of statistcs for this observation
	 { name = 'volume', 	
           key = {ipv4.client_ip, ipv4.server_ip},  
	   value = {ipv4.len}, 
	   features = {feature.SUM},
	   key_transforms = { {ipv4.client_ip, mapper.id_map, "client_usr"}, 
			      {ipv4.server_ip, mapper.geo_map, "server_location"} }		
        }
    }
}

------ initialize some state variables here ------
function init(detector_ref)
   
   -- containers for collecting the observation statistics
--   item_http_observer = Item:new(detector_ref, 'http-observation' ) 
     item_http_observer = Item:new(detector_ref, 'http-observation', 'volume')
end

function threshold_function(detector_ref)
   print ("Threshold function called");
   item_http_observer:range_query(types.metric.SUM)
   item_http_observer:print_like_json()	 	
--   item_http_observer:point_query_now()
--    data = item_http_observer:get_data("volume",types.metric.COUNT)
--    print (data);
   	-- item_http_observer:range_query(types.metric.SUM)

--	 item_http_observer:print_like_json("volume",types.metric.SUM);   
	 --item_http_observer:print_observation("volume",types.metric.SUM);
end

function export_function(detector_ref)
	--print("export_function called")
end
