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
# Author: Saman Taghavi Zargar <staghavi@cisco.com> 
#
#
--]]

-- import the types
require "types"
require "service_configurations"
local ipv4 = types.key_value_type.ipv4
local feature = types.metric
local mapper = service_configurations

-- UDP address scan detector detects that a host appears to be scanning some number of destinations
-- on a single port. In doing so, whenever more than "address_scan_threshold" unique hosts are seen
-- over previous "address_scan_interval".

local address_scan_threshold = 25
-- address_scan_interval is 5 minutes 
local address_scan_interval = 300

-- UDP port scan detector detects an attacking host appears to be scanning a single victim host on 
-- several ports. In doiong so, whenever an attacking host attempts to connect to "port_scan_threshold"
-- unique ports on a single host over the previous "port_scan_interval".

local port_scan_threshold = 15
-- port_scan_interval is 5 minutes 
local port_scan_interval = 300

----- configure package parameters -------
DetectorPackageInfo = {
    name =  'udp-scan-detector',
    init =  'init',
    observation_list_names = { 'udp_address_scan_observer', 'udp_port_scan_observer' },
    timed_callbacks = { {name = 'threshold_function', duration = 300},
                        {name = 'export_function', duration = 30} }

}

-- configure observation events ------------ 

udp_address_scan_observer  = {
    name = 'udp-address-observation',
    event_filter_rule_header = "udp any any <> any any",

    event_filter_rule_options = "",
    event_filter_rule_global_sid = "sid:1000006; rev:1",

    statistics_list = {
          -- list of statistcs for this observation
         { name = 'scanudp.addr.fail',
           key = {ipv4.client_ip, ipv4.server_port},
           value = {ipv4.server_ip},
           features = {feature.UNIQUE},
           key_transforms = { {ipv4.client_ip, mapper.id_map, "client_usr"},
                              {ipv4.server_ip, mapper.geo_map, "server_location"} }
        }
    }
}

udp_port_scan_observer  = {
    name = 'udp-port-observation',
    event_filter_rule_header = "udp any any <> any any",

    event_filter_rule_options = "",
    event_filter_rule_global_sid = "sid:1000006; rev:1",

    statistics_list = {
          -- list of statistcs for this observation
         { name = 'scanudp.port.fail',
           key = {ipv4.client_ip, ipv4.server_ip},
           value = {ipv4.server_port},
           features = {feature.UNIQUE},
           key_transforms = { {ipv4.client_ip, mapper.id_map, "client_usr"},
                              {ipv4.server_ip, mapper.geo_map, "server_location"} }
        }
    }
}

------ initialize some state variables here ------
function init(detector_ref)

   -- containers for collecting the observation statistics
     item_udp_address_scan_observer = Item:new(detector_ref, 'udp-address-observation', 'scanudp.addr.fail')
     item_udp_port_scan_observer = Item:new(detector_ref, 'udp-port-observation', 'scanudp.port.fail')
end

function threshold_function(detector_ref)
   print ("Threshold function called");
   local curMeasurements = { }
   udp-address-observation:range_query()
   udp-port-observation:range_query()

   local curMeasurements_time = udp-address-observation:get_st()- basetime

   curMeasurements_UniqueAddressScanned = udp-address-observation:get_data("scanudp.addr.fail",types.metric.UNIQUE)
   curMeasurements_UniquePortScanned = udp-port-observation:get_data("scanudp.port.fail",types.metric.UNIQUE)

   -- We need some helper functions that let the user simply write threshold related stuff. Preferably some funtions that would
   -- do perform queries on rollup item and generate an indexed list (accessable) (maybe sorted) of items that pass certain 
   -- thresholds that all the fields (key valuee and feature values) are accessable to be used by user in writing perfect scripts
   -- for instance: I would like to be able to do something like this:
   if (curMeasurements_UniqueAddressScanned  > address_scan_threshold) then
      print ("%s scanned at least %d unique hosts on UDP port %s in %s", curMeasurements_UniqueAddressScanned_client_ip, curMeasurements_UniqueAddressScanned_unique, curMeasurements_UniqueAddressScanned_port, curMeasurements_UniqueAddressScanned_duration);

   -- What bro does:
   if (curMeasurements.UniqueAddressScanned.unique  > address_scan_threshold) then
      print ("%s scanned at least %d unique hosts on UDP port %s in %s", key$host, r$unique, key$str, dur);
end

function export_function(detector_ref)
        --print("export_function called")
end


