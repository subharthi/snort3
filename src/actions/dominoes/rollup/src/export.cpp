// Author: Sam Zargar <staghavi@cisco.com>

#include "export.h"

using namespace rapidjson;

Export::Export(DetectorUserData* det_ref, std::string obs_name, std::string accum_name, std::string json_file):lasttimeCalled(0), now(0)
{
    json_file_name = json_file;
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

void Export::fill_result_table(metric::metric_type feature){

        std::vector<std::string> key_list_temp =  observation->get_key_list();
        std::vector<Key_Transform>  key_transform_list_temp = observation->get_key_transform_list();
        std::vector<std::string> metric_list_temp = observation->get_metrics_list();

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
                                         temp_result.first = metric_list_temp[0];
                                         temp_result.second = extract_result(feature, k.second);
                                         result_table.append(temp_key_vec, temp_result);
                         }
                }

           }



}

int Export::print_json_point(metric::metric_type feature){

    std::ofstream outfile;
    outfile.open (json_file_name, std::ios_base::app);
    
    time_t et = time(NULL);
    time_t st = et - dominoes_tick.tv_sec;

    
    //For testing only
    /*for(auto& iter_observation: detector_ref2->observation_list) {
          if (iter_observation->get_observation_name() == observation_name ) {
                  item->clear();
                  item->setEndTime(et);
                  item->setStartTime(st);
                  iter_observation->get_rollup().pointQuery(et, item);
                  fill_result_table(feature);
          }
    }*/
    
    for(auto& iter_observation: detector_ref->pDetector->observation_list) {
          std::cout<< "got in first loop" << std::endl;
          if (iter_observation->get_observation_name() == observation_name ) {
                  std::cout<< "got in if statment" << std::endl;
                  item->clear();
                  item->setEndTime(et);
                  item->setStartTime(st);
                  iter_observation->get_rollup().pointQuery(et, item);
                  fill_result_table(feature);
          }
    }

    for (auto& result_pair: result_table.get_result_map()){

          std::cout<< "got in second loop" << std::endl;
          StringBuffer os;
          Writer<StringBuffer> writer(os);
          writer.StartObject();
    	  writer.String("script_name");
    	  writer.String(detector_name.c_str());
    	  writer.String("observation_name");
    	  writer.String(observation_name.c_str());
    	  writer.String("accumulator_name");
    	  writer.String(accumulator_name.c_str());
    	  writer.String("start_time");
    	  writer.Uint(item->getStartTime());
    	  writer.String("end_time");
    	  writer.Uint(item->getEndTime());
     	  for (auto& key :result_pair.first){
                 writer.String(std::string(key.first).c_str());
                 writer.String(std::string(key.second).c_str());
	  }
          writer.String(std::string(result_pair.second.first).c_str());
          writer.Uint(result_pair.second.second);
          writer.EndObject();
          outfile << os.GetString() << std::endl;
          if (!outfile.good()){
              throw std::runtime_error ("Can't write the JSON string to the file!");
          }
    }
    outfile.close();
    return 0;
}


int Export::print_json_point(metric::metric_type feature, time_t s){

    std::ofstream outfile (json_file_name, std::ios_base::app);

    time_t et = s;
    time_t st = et - dominoes_tick.tv_sec;

    int ret = -1;

    for(auto& iter_observation: detector_ref->pDetector->observation_list) {
          if (iter_observation->get_observation_name() == observation_name ) {
                  item->clear();
                  item->setEndTime(et);
                  item->setStartTime(st);
                  iter_observation->get_rollup().pointQuery(et, item);
                  fill_result_table(feature);
          }
    }
    for (auto& result_pair: result_table.get_result_map()){
          StringBuffer os;
          Writer<StringBuffer> writer(os);
          writer.StartObject();
          writer.String("script_name");
          writer.String(detector_name.c_str());
          writer.String("observation_name");
          writer.String(observation_name.c_str());
          writer.String("accumulator_name");
          writer.String(accumulator_name.c_str());
          writer.String("start_time");
          writer.Uint(item->getStartTime());
          writer.String("end_time");
          writer.Uint(item->getEndTime());
          for (auto& key :result_pair.first){
                 writer.String(std::string(key.first).c_str());
                 writer.String(std::string(key.second).c_str());
          }
          writer.String(std::string(result_pair.second.first).c_str());
          writer.Uint(result_pair.second.second);
          writer.EndObject();
          outfile << os.GetString() << std::endl;
          if (!outfile.good()){
              throw std::runtime_error ("Can't write the JSON string to the file!");
          }
    }
    return 0;
}

int Export::print_json_range(metric::metric_type feature){

    std::ofstream outfile (json_file_name, std::ios_base::app);
    
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
                  fill_result_table(feature);
          }
    }
    for (auto& result_pair: result_table.get_result_map()){
          StringBuffer os;
          Writer<StringBuffer> writer(os);
          writer.StartObject();
          writer.String("script_name");
          writer.String(detector_name.c_str());
          writer.String("observation_name");
          writer.String(observation_name.c_str());
          writer.String("accumulator_name");
          writer.String(accumulator_name.c_str());
          writer.String("start_time");
          writer.Uint(item->getStartTime());
          writer.String("end_time");
          writer.Uint(item->getEndTime());
          for (auto& key :result_pair.first){
                 writer.String(std::string(key.first).c_str());
                 writer.String(std::string(key.second).c_str());
          }
          writer.String(std::string(result_pair.second.first).c_str());
          writer.Uint(result_pair.second.second);
          writer.EndObject();
          outfile << os.GetString() << std::endl;
          if (!outfile.good()){
              throw std::runtime_error ("Can't write the JSON string to the file!");
          }
    }
    return 0;
    lasttimeCalled = now;
}

int Export::print_json_range(metric::metric_type feature, time_t s, time_t e){

    std::ofstream outfile (json_file_name, std::ios_base::app);

    time_t et = e;
    time_t st = s;
    
    int ret = -1;

    for(auto& iter_observation: detector_ref->pDetector->observation_list) {
          if (iter_observation->get_observation_name() == observation_name ) {
                  item->clear();
                  item->setEndTime(et);
                  item->setStartTime(st);
                  iter_observation->get_rollup().rangeQueryBottomUp(st, et, item);
                  fill_result_table(feature);
          }
    }
    for (auto& result_pair: result_table.get_result_map()){
          StringBuffer os;
          Writer<StringBuffer> writer(os);
          writer.StartObject();
          writer.String("script_name");
          writer.String(detector_name.c_str());
          writer.String("observation_name");
          writer.String(observation_name.c_str());
          writer.String("accumulator_name");
          writer.String(accumulator_name.c_str());
          writer.String("start_time");
          writer.Uint(item->getStartTime());
          writer.String("end_time");
          writer.Uint(item->getEndTime());
          for (auto& key :result_pair.first){
                 writer.String(std::string(key.first).c_str());
                 writer.String(std::string(key.second).c_str());
          }
          writer.String(std::string(result_pair.second.first).c_str());
          writer.Uint(result_pair.second.second);
          writer.EndObject();
          outfile << os.GetString() << std::endl;
          if (!outfile.good()){
              throw std::runtime_error ("Can't write the JSON string to the file!");
          }
    }
    return 0;
}

