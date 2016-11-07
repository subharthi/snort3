// Author: Sam Zargar <staghavi@cisco.com>

#include "export.h"

using namespace rapidjson;

//To be removed as this will be passed through lua script
#define DOMINOES_PATH "/home/snortplus/snortplus/snort3/dominoes_json/"

Export::Export(DetectorUserData* det_ref, std::string obs_name, std::string accum_name, std::string json_file):lasttimeCalled(0), now(0)
{
    json_file_name = json_file;
    json_file_name = DOMINOES_PATH + json_file;	
    item = new Item();
    detector_ref = det_ref;
    detector_name = detector_ref->pDetector->get_detector_package_info().detector_name;
    observation_name = obs_name;
    accumulator_name = accum_name;
    for(auto& observation_iter: detector_ref->pDetector->observation_list) {
                if (observation_iter->get_observation_name() == observation_name ) {
                        observation = observation_iter;
                        break;
                }
    }
}

//For testing only
Export::Export(std::shared_ptr<Detector> det_ref2, std::string json_file):lasttimeCalled(0), now(0)
{
    json_file_name = json_file;
    json_file_name = DOMINOES_PATH + json_file;	
    item = new Item();
    detector_ref2 = det_ref2;
    detector_name = detector_ref2->get_detector_package_info().detector_name;
    observation_name = "http-observation";
    accumulator_name = "volume";
    for(auto& observation_iter: detector_ref2->observation_list) {
                if (observation_iter->get_observation_name() == observation_name ) {
                        observation = observation_iter;
                        break;
                }
    }
}

static std::vector<std::string>  tokenize_key(std::string key){
        std::vector<std::string> tokens;
        std::istringstream iss(key);
        copy(std::istream_iterator<std::string>(iss),
                std::istream_iterator<std::string>(),
                back_inserter(tokens));
        return tokens;

}

static std::string stringify_key(std::string key, std::string label)
{

    if (label.find("ip")){
        std::istringstream strm(key);
        char buffer[16];
        uint64_t ip;
        strm >> ip;
        unsigned char bytes[8];
        bytes[3] = ip & 0xFFFF;
        bytes[2] = (ip >> 8) & 0xFFFF;
        bytes[1] = (ip >> 16) & 0xFFFF;
         bytes[0] = (ip >> 24) & 0xFFFF;
        sprintf(buffer, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
        return std::string(buffer);
    } else {
        return key;
    }


}

void Export::fill_result_table(){

        std::vector<std::string> key_list_temp =  observation->get_key_list();
        std::vector<Key_Transform>  key_transform_list_temp = observation->get_key_transform_list();
        std::vector<std::string> metric_list_temp = observation->get_metrics_list();

	std::vector<metric::metric_type> feature_list_temp = observation->get_feature_list();

        // clear the table
        result_table.clear();

        for (auto &i : item->getData()) {
                if(std::string(i.first) == accumulator_name){
			for( auto& k : i.second.data ) {
				KeyVec temp_key_vec;
                                Result temp_result;
                                std::vector<std::string> tokens;
                                tokens = tokenize_key(k.first);
                                for (int j = 0; j< tokens.size(); j++){
                                        Key primary_key_temp;
                                        primary_key_temp.first = key_list_temp[j];
                                        primary_key_temp.second = stringify_key(tokens[j], key_list_temp[j]);
                                        temp_key_vec.push_back(primary_key_temp);
                                        if(key_transform_list_temp.size()){
                                                for (auto& transform: key_transform_list_temp){
                                                        if(transform.keystring == key_list_temp[j]){
                                                                Key derived_key_temp;
                                                                derived_key_temp.first = transform.meta_keystring;
                                                                derived_key_temp.second = transform(primary_key_temp.second);
                                                                temp_key_vec.push_back(derived_key_temp);
                                                          }
                                                 }
                                         }

                                }
				for (int l = 0; l< metric_list_temp.size(); l++){
                                     for (int m = 0; m < feature_list_temp.size(); m++){
                                          if (metric::metric_type_decoder(feature_list_temp[m]) == metric_list_temp[l]) {
                                                  temp_result.first = metric_list_temp[l];
                                                  temp_result.second = extract_result(feature_list_temp[m], k.second);
                                                  result_table.append(temp_key_vec, temp_result);
                                          }      
                                     }
                                }
                         }
                }

           }

}

void Export::add_json_schema_header(std::string json_file_name){
          
          std::ofstream outfile;
          outfile.open (json_file_name, std::ios_base::app);
          StringBuffer os;
          Writer<StringBuffer> writer(os);
          writer.StartArray();
          writer.StartObject();
          writer.String("$schema");
          writer.String("http://json-schema.org/draft-04/schema#");
          writer.String("description");
          writer.String("schema for Dominoes observations");
          writer.String("type");
          writer.String("JSON object");
          writer.String("properties");
          writer.StartObject();
          writer.String("script_name");
          writer.StartObject();
          writer.String("type");
          writer.String("string");
          writer.EndObject();
          writer.String("observation_name");
          writer.StartObject();
          writer.String("type");
          writer.String("string");
          writer.EndObject();
          writer.String("accumulator_name");
          writer.StartObject();
          writer.String("type");
          writer.String("string");
          writer.EndObject();
          writer.String("start_time");
          writer.StartObject();
          writer.String("type");
          writer.String("integer");
          writer.EndObject();
          writer.String("end_time");
          writer.StartObject();
          writer.String("type");
          writer.String("integer");
          writer.EndObject();
          writer.String("client_ip");
          writer.StartObject();
          writer.String("type");
          writer.String("IP address");
          writer.EndObject();
          writer.String("client_user");
          writer.StartObject();
          writer.String("type");
          writer.String("username");
          writer.EndObject();
          writer.String("server_ip");
          writer.StartObject();
          writer.String("type");
          writer.String("IP address");
          writer.EndObject();
          writer.String("server_location");
          writer.StartObject();
          writer.String("type");
          writer.String("GEO location");
          writer.EndObject();
          writer.String("oneOf");
          writer.StartArray();
          writer.StartObject();
          writer.String("$ref");
          writer.String("#/definitions/sum");
          writer.EndObject();
          writer.StartObject();
          writer.String("$ref");
          writer.String("#/definitions/count");
          writer.EndObject();
          writer.EndArray();
          writer.EndObject();
          writer.String("definitions");
          writer.StartObject();
          writer.String("sum");
          writer.StartObject();
          writer.String("properties");
          writer.StartObject();
          writer.String("type");
          writer.String("integer");
          writer.EndObject();
          writer.EndObject();
          writer.String("count");
          writer.StartObject();
          writer.String("properties");
          writer.StartObject();
          writer.String("type");
          writer.String("integer");
          writer.EndObject();
          writer.EndObject();
          writer.EndObject();
          writer.EndObject();
          writer.EndArray();
          outfile << os.GetString() << std::endl;
          if (!outfile.good()){
              throw std::runtime_error ("Can't write the JSON string to the file!");
          }
}

int Export::print_json_point(metric::metric_type feature){
    
    // Add JSON Schema header to the newly created files
    if (!(std::ifstream(json_file_name))){
          add_json_schema_header(json_file_name);
    }

    time_t et = time(NULL);
    time_t st = et - dominoes_tick.tv_sec;

    
    //For testing only
    /*for(auto& iter_observation: detector_ref2->observation_list) {
          if (iter_observation->get_observation_name() == observation_name ) {
                  item->clear();
                  item->setEndTime(et);
                  item->setStartTime(st);
                  iter_observation->get_rollup().pointQuery(et, item);
                  fill_result_table();
          }
    }*/
    
    for(auto& iter_observation: detector_ref->pDetector->observation_list) {
          if (iter_observation->get_observation_name() == observation_name ) {
                  item->clear();
                  item->setEndTime(et);
                  item->setStartTime(st);
                  iter_observation->get_rollup().pointQuery(et, item);
                  fill_result_table(feature);
          }
    }

    Document d, d2;
    FILE* fp = fopen(json_file_name.c_str(), "r");
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));   
    
    d.ParseStream(is);
    assert(d.IsArray());
    fclose(fp);
    d2.SetObject();
    Value json_objects(kObjectType);

    for (auto& result_pair: result_table.get_result_map()){
         Value json_objects(kObjectType);
          json_objects.AddMember("script_name", detector_name, d2.GetAllocator());
          json_objects.AddMember("observation_name", observation_name, d2.GetAllocator());
          json_objects.AddMember("accumulator_name", accumulator_name, d2.GetAllocator());
          json_objects.AddMember("start_time", item->getStartTime(), d2.GetAllocator());
          json_objects.AddMember("end_time", item->getEndTime(), d2.GetAllocator());
          for (auto& key :result_pair.first){
                 json_objects.AddMember(StringRef(key.first), StringRef(key.second), d2.GetAllocator());
          }
          
          if (std::string(result_pair.second.first).c_str() == "COUNT" or "SUM"){
              json_objects.AddMember(StringRef(result_pair.second.first), result_pair.second.second, d2.GetAllocator());
          }
          d.PushBack(json_objects, d2.GetAllocator());	
    }

    FILE* outfile = fopen(json_file_name.c_str(), "w");
    char writeBuffer[65536];
    FileWriteStream os(outfile, writeBuffer, sizeof(writeBuffer));

    Writer<FileWriteStream> writer(os); 
    d.Accept (writer);
    fclose(outfile);    
    return 0;
}


int Export::print_json_point(metric::metric_type feature, time_t s){

    // Add JSON Schema header to the newly created files
    if (!(std::ifstream(json_file_name))){
          add_json_schema_header(json_file_name);
    }

    time_t et = s;
    time_t st = et - dominoes_tick.tv_sec;

    int ret = -1;

    for(auto& iter_observation: detector_ref->pDetector->observation_list) {
          if (iter_observation->get_observation_name() == observation_name ) {
                  item->clear();
                  item->setEndTime(et);
                  item->setStartTime(st);
                  iter_observation->get_rollup().pointQuery(et, item);
                  fill_result_table();
          }
    }

    Document d, d2;
    FILE* fp = fopen(json_file_name.c_str(), "r");
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    d.ParseStream(is);
    assert(d.IsArray());
    fclose(fp);
    d2.SetObject();
    Value json_objects(kObjectType);

    for (auto& result_pair: result_table.get_result_map()){
         Value json_objects(kObjectType);
         json_objects.AddMember("script_name", detector_name, d2.GetAllocator());
         json_objects.AddMember("observation_name", observation_name, d2.GetAllocator());
         json_objects.AddMember("accumulator_name", accumulator_name, d2.GetAllocator());
         json_objects.AddMember("start_time", item->getStartTime(), d2.GetAllocator());
         json_objects.AddMember("end_time", item->getEndTime(), d2.GetAllocator());
         for (auto& key :result_pair.first){
	      json_objects.AddMember(StringRef(key.first), StringRef(key.second), d2.GetAllocator());	
         }
	 if (std::string(result_pair.second.first).c_str() == "COUNT" or "SUM"){
	     json_objects.AddMember(StringRef(result_pair.second.first), result_pair.second.second, d2.GetAllocator());
	 }
	 d.PushBack(json_objects, d2.GetAllocator());	
    }

    FILE* outfile = fopen(json_file_name.c_str(), "w");
    char writeBuffer[65536];
    FileWriteStream os(outfile, writeBuffer, sizeof(writeBuffer));

    Writer<FileWriteStream> writer(os);
    d.Accept (writer);
    fclose(outfile);
    return 0;
}

int Export::print_json_range(metric::metric_type feature){

    // Add JSON Schema header to the newly created files
    if (!(std::ifstream(json_file_name))){
          add_json_schema_header(json_file_name);
    }
    
    now = time(NULL);
    time_t et = now;
    time_t st = et - dominoes_tick.tv_sec;
    if (lasttimeCalled != 0){
       st = lasttimeCalled; 
    }
    
    int ret = -1;

    for(auto& iter_observation: detector_ref->pDetector->observation_list) {
          if (iter_observation->get_observation_name() == observation_name ) {
                  item->clear();
                  item->setEndTime(et);
                  item->setStartTime(st);
                  iter_observation->get_rollup().rangeQueryBottomUp(st, et, item);
                  fill_result_table();
          }
    }

    Document d, d2;
    FILE* fp = fopen(json_file_name.c_str(), "r");
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    d.ParseStream(is);
    assert(d.IsArray());
    fclose(fp);
    d2.SetObject();
    Value json_objects(kObjectType);

    for (auto& result_pair: result_table.get_result_map()){
          Value json_objects(kObjectType);
          json_objects.AddMember("script_name", detector_name, d2.GetAllocator());
          json_objects.AddMember("observation_name", observation_name, d2.GetAllocator());
          json_objects.AddMember("accumulator_name", accumulator_name, d2.GetAllocator());
          json_objects.AddMember("start_time", item->getStartTime(), d2.GetAllocator());
          json_objects.AddMember("end_time", item->getEndTime(), d2.GetAllocator());
          for (auto& key :result_pair.first){
                json_objects.AddMember(StringRef(key.first), StringRef(key.second), d2.GetAllocator()); 
          }
          if (std::string(result_pair.second.first).c_str() == "COUNT" or "SUM"){
              json_objects.AddMember(StringRef(result_pair.second.first), result_pair.second.second, d2.GetAllocator());
          }
          d.PushBack(json_objects, d2.GetAllocator());
    }

    FILE* outfile = fopen(json_file_name.c_str(), "w");
    char writeBuffer[65536];
    FileWriteStream os(outfile, writeBuffer, sizeof(writeBuffer));

    Writer<FileWriteStream> writer(os);
    d.Accept (writer);
    fclose(outfile);
    lasttimeCalled = now;
    return 0;	
}

int Export::print_json_range(metric::metric_type feature, time_t s, time_t e){

    // Add JSON Schema header to the newly created files
    if (!(std::ifstream(json_file_name))){
          add_json_schema_header(json_file_name);
    }

    time_t et = e;
    time_t st = s;
    
    int ret = -1;

    for(auto& iter_observation: detector_ref->pDetector->observation_list) {
          if (iter_observation->get_observation_name() == observation_name ) {
                  item->clear();
                  item->setEndTime(et);
                  item->setStartTime(st);
                  iter_observation->get_rollup().rangeQueryBottomUp(st, et, item);
                  fill_result_table();
          }
    }

    Document d, d2;
    FILE* fp = fopen(json_file_name.c_str(), "r");
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    d.ParseStream(is);
    assert(d.IsArray());
    fclose(fp);
    d2.SetObject();
    Value json_objects(kObjectType);

    for (auto& result_pair: result_table.get_result_map()){
          Value json_objects(kObjectType);
          json_objects.AddMember("script_name", detector_name, d2.GetAllocator());
          json_objects.AddMember("observation_name", observation_name, d2.GetAllocator());
          json_objects.AddMember("accumulator_name", accumulator_name, d2.GetAllocator());
          json_objects.AddMember("start_time", item->getStartTime(), d2.GetAllocator());
          json_objects.AddMember("end_time", item->getEndTime(), d2.GetAllocator());
          for (auto& key :result_pair.first){
                 json_objects.AddMember(StringRef(key.first), StringRef(key.second), d2.GetAllocator());
          }
          if (std::string(result_pair.second.first).c_str() == "COUNT" or "SUM"){
              json_objects.AddMember(StringRef(result_pair.second.first), result_pair.second.second, d2.GetAllocator());
          }
          d.PushBack(json_objects, d2.GetAllocator());
    }
    FILE* outfile = fopen(json_file_name.c_str(), "w");
    char writeBuffer[65536];
    FileWriteStream os(outfile, writeBuffer, sizeof(writeBuffer));

    Writer<FileWriteStream> writer(os);
    d.Accept (writer);
    fclose(outfile);
    return 0;
}

