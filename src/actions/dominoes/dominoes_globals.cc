#include "lua/lua.h"
#include "lua/lua_util.h"

#include "dominoes_globals.h"

thread_local std::vector <std::shared_ptr<Detector> > detector_list;
thread_local uint32_t event_local_sid = 100000;

/* currently we print the error to stdout */ 
void LOG_LUA_ERROR_WITH_FILENAME_AND_EXIT(lua_State *L, std::string _file_name, std::string _message){
	std::cout << _message << " " << "Detector File: " << _file_name << std::endl;
         lua_error(L);
         exit(-1);

}

void LOG_LUA_ERROR_AND_EXIT(lua_State *L, std::string _message){
	std::cout << _message << std::endl;
	lua_error(L);
	exit(-1);

}


void LOG_ERROR_WITH_FILENAME_AND_EXIT(std::string _file_name, std::string _message){
        std::cout << _message << " " << "Detector File: " << _file_name << std::endl;
         exit(-1);

}

void LOG_ERROR_AND_EXIT(std::string _message){
        std::cout << _message << std::endl;
        exit(-1);

}


