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
local types = types

------- local variables ------------------------
windowSize = 3

-- TODO: should be queried from the Dominoes instance
timeWindow = 3
max_threshold_limit = 10

-- tolerable anomaly window
tolerable_anomaly_window_count_flash_crowd = 2
tolerable_anomaly_window_count_syn_attack = 2 

-- epsilon values
epsilon_flash_crowd = 0.8
epsilon_syn_attack = 0.8 

--high watermark level to alert traffic high
high_watermark_level = 0.8

-- counters for syn attack and flash crowds
syn_attack_counter = 0
flash_crowd_counter = 0

-------- Log File setup -------------
logFile = "/var/log/syn-attack-stats.log"
logFileHandle = io.open(logFile,"w")
logFileHandle:write("Time(s),", "TCP Syn, ", "TCP Syn-Acks, ", "TCP Conns \n")
basetime = os.time()

-------------------------------------


----- configure package parameters -------
DetectorPackageInfo = {
    name =  'syn-attack-detector',
    init =  'init',
    observation_list_names = { 'observation_tcp_syns', 'observation_tcp_synacks', 'observation_tcp_conn_established' },
    tick_callback_fn_list = { name = "threshold_function", duration = 1}	
}

-- configure observation events ------------ 
observation_tcp_syns  = { 
    name = 'syn-observer', 
    event_filter_rule_header = "tcp any any -> any any", 
    event_filter_rule_options = "flags:S; ", 
    event_filter_rule_global_sid = "sid:14444; rev:1",
    statistics_list = {    
          -- list of statistcs for this observation
	 { name = 'total_count', 
	   key = {""},
	   value = {""}, 
	   features = {types.metric.COUNT}},
                        
	{ name = 'count_per_ip', 
	  key = {types.key_value_type.ipv4.src_ip}, 
	  value = {types.key_value_type.ipv4.ver}, 
          features = {types.metric.COUNT}} 
    }	 
}

observation_tcp_synacks = { 
    name = 'synack-observer',
    event_filter_rule_header = "tcp any any -> any any", 
    event_filter_rule_options = "flags:SA; ", 
    event_filter_rule_global_sid = "sid:14445; rev:1",
    statistics_list = {     
          -- list of statistcs for this observation
          { name = 'total_count',
            key = {""},
            value = {""},
            features = {types.metric.COUNT}}
    }
}	

observation_tcp_conn_established = { 
    name = 'conn-established-observer',
    event_filter_rule_header = "tcp any any -> any any",
    event_filter_rule_options = "flow:tcp_handshake_third_packet; ",                               
    event_filter_rule_global_sid = "sid:14446; rev:1",
    statistics_list = {     
          -- list of statistcs for this observation
          { name = 'total_count',
            key = {""},
            value = {""},
            features = {types.metric.COUNT}}
    }
}

------ initialize some state variables here ------
function init(detector_ref)
   
   -- containers for collecting the observation statistics
   syn_observer = Item.new(detector_ref, 'syn-observer' ) 
   synack_observer = Item.new(detector_ref, 'synack-observer')
   conn_observer = Item.new(detector_ref, 'conn-established-observer')

end


function threshold_function(detector_ref)
   local curMeasurements = { } 
   syn_observer:range_query()
   synack_observer:range_query()
   conn_observer:range_query() 

--   if ( syn_observer:get_data("total_count",types.metric.COUNT) > MAX_ACCEPTABLE_LOAD ) then
--        analyze_traffic()
--   end        
 
   curMeasurements.time = syn_observer:get_st()- basetime
   
   curMeasurements.tcpSyns = syn_observer:get_data("total_count",types.metric.COUNT) * 50
   curMeasurements.tcpSynAcks = synack_observer:get_data("total_count",types.metric.COUNT) * 25
   curMeasurements.tcpConns =  conn_observer:get_data("total_count",types.metric.COUNT) * 50
  
   print (curMeasurements.tcpSyns, " : ", curMeasurements.tcpSynAcks, " : ", curMeasurements.tcpConns);   
   if (logFileHandle ~= nil) then
      writeToLog(curMeasurements,logFileHandle) 
   end	
end


function analyze_traffic() 	
--   local currMeasurements = { }
   
--   synack_observer:range_query()
--   conn_observer:range_query() 
   
--   synack_tot_count = synack_observer:get_data("total_count",types.metric.COUNT)
--   conn_tot_count = conn_observer:get_data("total_count",types.metric.COUNT)

   -- Logic to identify whether it is a Syn Flood or a Flash Crowd 

end
        
	----- Log the data -----
--	curMeasurements.time = syn_count:get_st()- basetime
--        curMeasurements.tcpSyns = syn
--        curMeasurements.tcpSynAcks = syn_ack
--        curMeasurements.tcpConns = conn_established
 	
       -- print (curMeasurements.time, curMeasurements.tcpSyns, curMeasurements.tcpSynAcks, curMeasurements.tcpConns)

--	if (logFileHandle ~= nil) then
--                writeToLog(curMeasurements,logFileHandle)
--	else
--                print("Error opening log file")
--        end

--	if (syn >= (high_watermark_level * (max_threshold_limit*timeWindow))) then
--                print ("\n", "R1: ", R1, " - Traffic High Alert!! : Could be SYN Attack, or a Flash Crowd or Nothing\n")
		
		
	--	detector_api.query_current_window(synack_count, detector_ref, "synack-observer")
	--	detector_api.query_current_window(conn_established_count, detector_ref,"conn-established-observer")
		
	--	R2 = (conn_established_count:get_data("total_count",types.metric.COUNT) + 1)/(synack_count: get_data("total_count",types.metric.COUNT)+ 1) 
	--	if (R2 <= epsilon_syn_attack) then   -- syn attack seems imminent
	--		syn_attack_counter = syn_attack_counter + 1 
	--		print ("\n\t", "R2: ", R2, " - Traffic High Alert!! : A SYN Attack seems imminent\n")
	--		
	--	else --- high traffic, but no sign of syn attack
	--		if (syn_attack_counter > 0 ) then
	--			syn_attack_counter = syn_attack_counter  - 1
	--		end
	--	end
	--	
	--	R3 = (synack_count:get_data("total_count",types.metric.COUNT)+ 1)/(syn_count:get_data("total_count",types.metric.COUNT)+ 1)
	  --      if (R3 <=  epsilon_flash_crowd) then   -- flash crowd seems imminent
	--		flash_crowd_counter = flash_crowd_counter + 1
	--		print ("\n\t", "R3: ", R3,  " - Traffic High Alert!! : A Flash Crowd seems imminent\n")
	--	else
	--		if (flash_crowd_counter > 0) then
	--			flash_crowd_counter = flash_crowd_counter + 1
	--		end
	--	end
	--else
	--	if (syn_attack_counter > 0) then
	---		syn_attack_counter = syn_attack_counter - 1
	--	end			
	--	if (flash_crowd_counter > 0) then
	--		flash_crowd_counter  = flash_crowd_counter - 1
	--	end
	--	
	--end		

--	if (syn_attack_counter >  tolerable_anomaly_window_count_syn_attack) then
--		print ("\n\t SYN FLOOD ATTACK Detected!!")
  --      end
---
   --    if (flash_crowd_counter > tolerable_anomaly_window_count_flash_crowd) then
---		print ("\nFLASH CROWD: started dropping packets!!")
   --    end      
--	
--
	--detector_api.range_query_last_n(syn_count_sw, detector_ref, "syn-observer", windowSize)
	
        --detector_api.query_current_window(synack_count, detector_ref, "synack-observer")
	--detector_api.range_query_last_n(synack_count_sw, detector_ref, "synack-observer", windowSize)	
        
	--detector_api.query_current_window(conn_established_count, detector_ref,"conn-established-observer")
	--detector_api.range_query_last_n(conn_established_count_sw, detector_ref,"conn-established-observer",windowSize)

        
	--syn_count_sw_r= syn_count_sliding_window:get_data("total_count", types.metric.COUNT);
       
        
       --print()
     --  print ("Current Window: " ,syn_count_current_interval:get_start_time(), " : " , syn_count_current_interval:get_end_time(), " : ", syn_count_current_interval:get_interval()," : ",  syn_count_current)
     --  print ("Recent History: " ,syn_count_sliding_window:get_start_time(), " : " , syn_count_sliding_window:get_end_time(), " : ", syn_count_sliding_window:get_interval(), " : ", syn_count_past)
       
       --normalized_ratio =  (syn_count_current/syn_count_current_interval:get_interval())/ (syn_count_past/syn_count_sliding_window:get_interval())
       --print ("Normalized Ratio:" , normalized_ratio)
       --if (normalized_ratio < 1.5 ) then
	--	print ("Seems to be normal traffic")
       --else 
	--	print ("Seems to be experiencing abnormal number of connection requests")
       --end	
       --print()	
        
	--if item:threhold_();
	--detector_api.get_stats(detector_ref,"syn-ack-counts","count_half_open_conn_per_src_ip");


function writeToLog(_curMeasurements,_logFileHandle)

        if (_logFileHandle ~= nil) then
                _logFileHandle:write(tostring(_curMeasurements.time),
                                   ",",
                                   tostring(_curMeasurements.tcpSyns),
                                  ",",
                                  tostring(_curMeasurements.tcpSynAcks),
                                   ",",
                                   tostring(_curMeasurements.tcpConns),
                                   "\n")
                _logFileHandle:flush()
        else
                print ("LogFileHandle == Null")
        end
end

