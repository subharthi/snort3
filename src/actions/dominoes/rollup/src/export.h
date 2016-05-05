// Author: Sam Zargar <staghavi@cisco.com>

// Export class to create JSON stream and export it into various mediums (e.g., files, sockets, etc.)

#ifndef EXPORT_H
#define EXPORT_H

#include <boost/preprocessor.hpp>

#include "lua/lua.h"
#include "lua/lua_util.h"
#include "actions/dominoes/accumulators/src/accumulator_group.hpp"
#include "actions/dominoes/dominoes_detector_api.h"
#include "actions/dominoes/observation.h"
#include "actions/dominoes/rollup/src/rollup.h"
#include "actions/dominoes/rollup/src/export.h"
#include "actions/dominoes/lua_rollup_item_wrapper.h" 
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <fstream>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

struct DetectorUserData;
class Observation;
class Detector;

#include "actions/dominoes/accumulators/src/metrics.hpp"

class Export
{
    Item* item;
    DetectorUserData* detector_ref;
    std::shared_ptr<Detector> detector_ref2;
    std::string observation_name;
    std::string detector_name;
    std::string accumulator_name;
    std::shared_ptr<Observation> observation;
    time_t lasttimeCalled;
    time_t now;
    ResultTable result_table;
    std::string json_file_name;
public:
    Export():lasttimeCalled(0), now(0){}
    Export(DetectorUserData* det_ref, std::string obs_name, std::string accum_name, std::string json_file);
    
    //Test only constor
    Export(std::shared_ptr<Detector> det_ref2, std::string json_file);
     
    void fill_result_table(metric::metric_type  feature);
    int print_json_point(metric::metric_type feature);
    int print_json_point(metric::metric_type feature, time_t s);

    int print_json_range(metric::metric_type feature);
    int print_json_range(metric::metric_type feature, time_t s, time_t e);
    //int json_to_file_pretty();

};

#endif
