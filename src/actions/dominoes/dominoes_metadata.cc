/*
** Copyright (C) 2014 Cisco and/or its affiliates. All rights reserved.
** Copyright (C) 2005-2013 Sourcefire, Inc.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/



/*
 ** dominoes_metadata.cc  Author:Subharthi Paul <subharpa@cisco.com>
 */


#include "dominoes_metadata.h"
#include<iostream>

void Metadata_Acces_Object::insert(Metadata_Object mo){ 
	metadata_object_list.push_back(mo);
}

Metadata_Object&  Metadata_Acces_Object::get_metadata_object(std::string& name) {
                for(auto& metadata_object : metadata_object_list){
                        if (metadata_object.get_name() == name )
                                return metadata_object;
                }
		//TODO: Handle this more graciously
		// Currently exit because other parts of the code will fail
                std::cout<< "ERROR: configured metadata object not found " << std::endl;
		exit(-1);
}


/*	
uint64_t  Dictionary::insert(std::string str){ 
		auto search = dict.find(str);
		if (search == dict.end()){
			// insert into the dictionary
			uint64_t new_index = dict.size() + 1;
			dict.insert(std::pair<std::string,uint64_t>(str,new_index)) ;
			return new_index;
		} else {
			return search->second;
		}
}	
	
uint64_t  Dictionary::get_val(std::string str){
		auto search =  dict.find(str);
		if (search != dict.end()){
			return search->second;	
		} else {
			return 0;
		}
}
	
std::string  Dictionary::get_key(uint64_t index){
		for(auto& dictionary_node: dict ){
			if (dictionary_node.second == index){
				return dictionary_node.first;
			}		
		}
		return NULL;
}
*/	
void  Metadata_Object::insert (std::string _key, std::string _val){
		metadata_store.insert(std::pair<std::string,std::string>(_key,_val));

}

void  Metadata_Object::set_name(std::string _metadata_object_name){
	metadata_object_name  = _metadata_object_name;
}

std::string&  Metadata_Object::get_name() {return metadata_object_name;}

bool  Metadata_Object::get_val(std::string key, std::string& val) {
	 	if (metadata_store.empty())
                return false;
        	auto search = metadata_store.find(key);
        	if( search == metadata_store.end() )
                	return false;
       		else {
                	val = search->second;
                	return true;	
		auto search = metadata_store.find(key);
		}
}
