--[[
#  Project Dominoes
#  Author: Subharthi Paul <subharpa@cisco.com>
--]]

-- common defintions file for Dominoes scripts

local key_value_type = {
	ipv4 = {
		hlen = 0,
        	ver = 1,
        	tos = 2,
        	len = 3,
      		ttl = 4,
        	proto = 5,
        	src_ip = 6,
        	dst_ip = 7,
		client_ip = 8,
		server_ip = 9,
		client_port = 10,
		server_port = 11
		},
	ipv6,
	tcp,
	udp	
}

local metric_type = {
	COUNT = 0,
	MAX = 1,
	MIN = 2,
	SUM = 3,
	MEAN = 4,
	VARIANCE = 5,
	MOMENT_1 = 6,
	SKEWBESS = 7,
	MEDIAN = 8,
	KURTOSIS = 9
}
--local metric_type = {
--	COUNT = 0,
--	KURTOSIS = 1,
--	MAX = 2,
--	MEAN = 3,
--        MEDIAN = 4,
--	MIN = 5,
--	SKEWNESS = 6,
--	SUM = 7
--} 

--[[ Exporting functions and data in this package. Function/data not listed here will remain 
--hidden from other lua files. Any helper functions that are needed often should be moved
--into this package.
--]]

types = {

	key_value_type = key_value_type,
	metric = metric_type
}

