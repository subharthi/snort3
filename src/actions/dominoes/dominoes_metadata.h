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
 ** dominoes_metadata.h  Author:Subharthi Paul <subharpa@cisco.com>
 */


// For now we assume the metadata to be a generic kv store

#ifndef _DOMINOES_METADATA_H_
#define  _DOMINOES_METADATA_H_

#include <vector>
#include <map>

class Metadata_Object;

class Metadata_Acces_Object{
private:
	std::vector<Metadata_Object> metadata_object_list;

public:
	Metadata_Object& get_metadata_object(std::string& name);
	void insert(Metadata_Object mo);
};
/*
class Dictionary{

	std::map<std::string, uint64_t> dict;
	
	public:
	uint64_t insert(std::string str);  
	uint64_t get_val(std::string str);
        std::string get_key(uint64_t index);
};
*/
class Metadata_Object{
	
	private:
	
	std::string metadata_object_name;
	std::map<std::string, std::string> metadata_store;
	//Dictionary key_dictionary;
	//Dictionary val_dictionary;
	

 	public:
        void insert (std::string _key, std::string _val);
	void set_name(std::string _metadata_object_name);
	std::string& get_name();
	bool get_val(std::string key, std::string& val);
};

#endif
