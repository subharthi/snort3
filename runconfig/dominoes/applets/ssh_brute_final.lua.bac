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
#
# Detecting an increasing total number of ssh login failures with in a domain, then per ssh server 
# , and finally hosts that are doing password guessing attacks and/or password
# bruteforcing over SSH.
#
--]]


-- import the types
require "types"
require "service_configurations"
local ipv4 = types.key_value_type.ipv4
local feature = types.metric
local mapper = service_configurations

basetime = os.time()

-- The number of failed SSH connections before an attempter is detected as a password guesser. 
-- The ideal goal is to detect slow (stealthy) attackers as for shorter period of time and more aggressive ones 
-- Snort probably does a very good job. Good threshold would be 30 attempts for the duration of 30 minutes.

local password_guess_threshold_per_client = 3

-- How many total ssh login failures per server? Again consider the case of bots bruteforcing one server through geniune 
-- 100,000 attempts? It certainly doesnt pass the per client threshold to raise an alarm! 

local total_tolerable_threshold_per_server = 2

-- How many total ssh login failures in our network before we react and perform further analysis? Also what is 100,000
-- bots send us only couple of ssh login attempts (accumulative bruteforce)? 

local total_tolerable_threshold = 1


-- The amopunt of time to keep the number of unsuccessful login attempts in order to build
-- a model for the password guessers.

-- HINT: Dominoes minimum time tick is fixed to 10seconds and the numbers that should be provided here will be multiplied by 10 seconds
-- E.g., 10 means 10 * 10 = 100 seconds
-- address_scan_interval could be longer like 30 minutes (1800seconds / 10sec(default minimum tick))

local bruteforce_modeling_timeout_per_client = 10

-- The amopunt of time to keep the number of unsuccessful login attempts per server in order to build
-- a model for reacting to the bots causing the bruteforce

local bruteforce_modeling_timeout_per_server = 5


-- The amopunt of time to keep the number of unsuccessful login attempts in total in order to build
-- a model for reacting to the password guessers.

local bruteforce_modeling_timeout_total = 2

-- Exclusion of certain subnets from being tracked as guessers. 

--local exclusion_list = {192.168.0.0/24}

----- configure package parameters -------
DetectorPackageInfo = {
    name =  'detector',
    init =  'init',
    observation_list_names = { 'ssh_password_guesser_observer' },
    timed_callbacks = { {name = 'threshold_function', duration = bruteforce_modeling_timeout_total},
                        {name = 'export_function', duration = bruteforce_modeling_timeout_total} }

}

-- configure observation events ------------ 

ssh_password_guesser_observer  = {
    name = 'ssh-bruteforce-observation',
    --event_filter_rule_header = "tcp 10.10.105.2 any -> 10.10.106.2 22",
    event_filter_rule_header = "tcp $EXTERNAL_NET any -> $HOME_NET 22",

    -- The big (obvious) idea is the better you can write the signature here (i.e., more comprehensive signature)
    -- the better you filter out unnecessary packets to analyze  
--    event_filter_rule_options = "flags: S;",
    event_filter_rule_options = "flow:to_server,established; content:\"SSH\",nocase, offset 0, depth 4;",
    event_filter_rule_global_sid = "sid:19559; rev:1",

    statistics_list = {
          -- list of statistcs for this observation

          -- Lets gather total number of ssh login attempts   
         { name = 'total.ssh.login.failure',
           key = {ipv4.client_ip},
           value = {ipv4.server_ip},
           features = {feature.COUNT},
           -- No need for transform as this step is to just trigger the further analysis
           key_transforms = { }
         },

         -- Lets gather total number of ssh login attempts per ssh server_ip    
        { name = 'total.ssh.login.failure.per.server',
           key = { },
           value = {ipv4.server_ip},
           features = {feature.COUNT},
           -- Still not need to transform the keys to get further info
           key_transforms = { }
        },

         -- Lets gather number of ssh login attempts for each client_ip per ssh server_ip       
        { name = 'total.ssh.login.failure.each.client.per.server',
           key = {ipv4.client_ip},
           value = {ipv4.server_ip},
           features = {feature.COUNT},
           -- Lets transform the keys to info available through other resources to match/get more info on attack actors         
           key_transforms = { {ipv4.client_ip, mapper.id_map, "client_usr"},
                              {ipv4.server_ip, mapper.geo_map, "server_location"} }
        }
    }
}

------ initialize some state variables here ------
function init(detector_ref)

   -- containers for collecting the observation statistics
     item_ssh_bruteforce_observer_total = Item:new(detector_ref, 'ssh-bruteforce-observation', 'total.ssh.login.failure')
     item_ssh_bruteforce_observer_per_server = Item:new(detector_ref, 'ssh-bruteforce-observation', 'total.ssh.login.failure.per.server')
     item_ssh_bruteforce_observer_each_client_per_server = Item:new(detector_ref, 'ssh-bruteforce-observation', 'total.ssh.login.failure.each.client.per.server')

end

function threshold_function(detector_ref)
   print ("Threshold function called");
   -- querying the rollup for past duration upto now
   item_ssh_bruteforce_observer_total:range_query_duration(types.metric.COUNT, bruteforce_modeling_timeout_total)
   local curMeasurements_total = { }
   curMeasurements_total = item_ssh_bruteforce_observer_total:get_data('total.ssh.login.failure',types.metric.COUNT)
   if (curMeasurements_total > total_tolerable_threshold) then
        print ("Appears that total number of failed SSH logins are increasing!");
        local time_duration
        time_duration = item_ssh_bruteforce_observer_total:get_st() - item_ssh_bruteforce_observer_per_server:get_st()
        if (time_duration >= bruteforce_modeling_timeout_per_server) then
                item_ssh_bruteforce_observer_per_server:range_query_duration(types.metric.COUNT, bruteforce_modeling_timeout_per_server)
                local curMeasurements_per_server = { }
                curMeasurements_per_server = item_ssh_bruteforce_observer_per_server:get_data('total.ssh.login.failure.per.server',types.metric.COUNT)
                if (curMeasurements_per_server > total_tolerable_threshold_per_server) then
                        print ("Appears that total number of failed SSH logins per server is increasing for the following servers!");
                        item_ssh_bruteforce_observer_per_server:print_like_json()
                        local time_duration_per_client_case = item_ssh_bruteforce_observer_total:get_st() - item_ssh_bruteforce_observer_each_client_per_server:get_st()
                        if (time_duration_per_client_case >= bruteforce_modeling_timeout_per_client) then
                                item_ssh_bruteforce_observer_each_client_per_server:range_query_duration(types.metric.COUNT, bruteforce_modeling_timeout_per_client)
                                local curMeasurements_each_client_per_server = { }
                                curMeasurements_each_client_per_server = item_ssh_bruteforce_observer_each_client_per_server:get_data('total.ssh.login.failure.each.client.per.server',types.metric.COUNT)
                                if (curMeasurements_each_client_per_server > password_guess_threshold_per_client) then
                                        print ("Appears that total number of failed SSH logins for these clients per server is pasing the threshold!");
                                        item_ssh_bruteforce_observer_each_client_per_server:print_like_json()
                                end
                        end
                end
        end
   end



end

function export_function(detector_ref)
        --print("export_function called")
end

