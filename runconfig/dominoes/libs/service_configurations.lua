--[[
#  Project Dominoes
#  Author: Subharthi Paul <subharpa@cisco.com>
--]]

-- common defintions file for Dominoes scripts

local id_map = { "ip_user_mapping", "/home/vagrant/dominoes3/runconfig/mappings/ISE", "file" }
local  geo_map = {"ip_geolocation_mapping", "/home/vagrant/dominoes3/runconfig/mappings/GEO", "file"}  

service_configurations = {
	id_map = id_map;
	geo_map = geo_map
}
