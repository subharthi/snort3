#ifndef DOMINOES_LUA_MACROS_H
#define DOMINOES_LUA_MACROS_H


#define LUA_GET_TABLE_START(LUA_CONTEXT, TABLE_NAME, REQUIRED_FLAG, DEFAULT_VAL) 	\
	lua_getglobal (LUA_CONTEXT, TABLE_NAME);								\
	if (!lua_istable(LUA_CONTEXT, -1)) {									\
            	if (REQUIRED_FLAG){										\
			std::cout<< TABLE_NAME << " is required" << std::endl; 					\
			exit(-1);										\
		}												\
		lua_pop(LUA_CONTEXT, 1);									\
                return;												\
        }													\


#define LUA_GET_FIELD_STRING(LUA_CONTEXT, FIELD_NAME, PARENT_STRUCT_PTR, FIELD_VAR, REQUIRED_FLAG, DEFAULT_VAL)		\
	lua_getfield(LUA_CONTEXT, -1, FIELD_NAME); 										\
        if (lua_isstring(LUA_CONTEXT, -1)){										\
        	PARENT_STRUCT_PTR.FIELD_VAR = strdup(lua_tostring(LUA_CONTEXT, -1));					\
	} else	if (REQUIRED_FLAG){											\
			PARENT_STRUCT_PTR.FIELD_VAR = std::string(DEFAULT_VAL); 					\
		} else {												\
			std::cout<< FIELD_NAME << " is required" << std::endl; 						\
                        exit(-1);											\
	}														\
															\
        lua_pop(LUA_CONTEXT, 1);											\




#define LUA_GET_FIELD_STRING_VECTOR(LUA_CONTEXT, FIELD_NAME, PARENT_STRUCT_PTR, FIELD_VAR, REQUIRED_FLAG, DEFAULT_VAL) 	\
	lua_getfield(LUA_CONTEXT, -1, FIELD_NAME);									\
        if (lua_istable(LUA_CONTEXT, -1)){										\
                int size = luaL_getn(LUA_CONTEXT, -1);									\
                for (int i=1; i<= size; i++){										\
                        lua_rawgeti(LUA_CONTEXT,-1,i);									\
                        PARENT_STRUCT_PTR.FIELD_VAR.push_back(std::string(strdup(lua_tostring(LUA_CONTEXT,-1))));	\
                        lua_pop(LUA_CONTEXT,1);										\
                }													\
        }														\
        else if(REQUIRED_FLAG){												\
			PARENT_STRUCT_PTR.FIELD_VAR.push_back(std::string(DEFAULT_VAL)); 				\
		} else {												\
			 std::cout<< FIELD_NAME << " is required" << std::endl;                                  	\
                         exit(-1); 											\
	} 														\
        lua_pop(LUA_CONTEXT, 1);											\
			



#define LUA_GET_FIELD_TABLE_START(LUA_CONTEXT, TABLE_NAME, PARENT_STRUCT_PTR, FIELD_VAR, REQUIRED_FLAG, DEFAULT_VAL)  \
        lua_getglobal (LUA_CONTEXT, TABLE_NAME);                                                                \
        if (!lua_istable(LUA_CONTEXT, -1)) {                                                                    \
                if (REQUIRED_FLAG){                                                                             \
                        std::cout<< TABLE_NAME << " is required" << std::endl;                                  \
                        exit(-1);                                                                               \
                }                                                                                               \
                lua_pop(LUA_CONTEXT, 1);                                                                        \
                return;                                                                                         \
        }   

/*
#define LUA_GET_FIELD_TABLE_START(LUA_CONTEXT, FIELD_NAME, PARENT_STRUCT_PTR, FIELD_VAR, REQUIRED_FLAG, DEFAULT_VAL)  \
	lua_getfield(LUA_CONTEXT, -1, "tick_callback_fn"); //table
        if (lua_istable(L, -1)){
                uint32_t duration;
                lua_getfield(L, -1, "name"); // string 
                if (lua_isstring(L, -1)){
                        //TODO  Replace blank space with - in the name
                        pkg->tick_callback_fn = strdup(lua_tostring(L, -1));
                        if (!pkg->tick_callback_fn){
                                LogMessage("Error: syntax: tick_callback_fn = {name (string), duration(number)} %s", pkg->name.c_str());
                                exit(-1);
                        }
                } else  {
                        LogMessage("Error: syntax: tick_callback_fn = {name (string), duration(number)} %s", pkg->name.c_str());
                        exit(-1);
                }
                
                lua_pop(L, 1);
                lua_getfield(L, -1, "duration"); // number      
                if (lua_isnumber(L, -1)){
                         duration = lua_tonumber(L, -1);
                       
                }  else {
                        LogMessage("Error: syntax: tick_callback_fn = {name (string), duration(number)} %s", pkg->name.c_str());
                        exit (-1);
               }
               detector->detector_timed_callback_list.push_back(new Detector_Timed_Callback(pkg->tick_callback_fn, duration, L));
               lua_pop(L, 1);
        }
        else {
                //TODO: add a default selector
                LogMessage("The callback function has to provide a callback function name and duration %s", pkg->name.c_str());
                exit (-1);
        }

*/







#define LUA_GET_TABLE_END(LUA_CONTEXT) lua_pop(LUA_CONTEXT, 1); 


#endif



	
