#ifndef ROLLUP_H
#define ROLLUP_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <list>
#include <string>
#include "actions/dominoes/accumulators/src/statistics.hpp"
#include <fstream>
#include <ios>

//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>

#include <boost/circular_buffer.hpp>

class Item
{

    time_t startTime;
    time_t endTime;
    accumulator_table_map_type  data;
   // int sum;
public:
    Item():startTime(0), endTime(0){}
    
    Item(time_t st, time_t et, accumulator_table_map_type d);
   // :startTime(st), endTime(et), data(d){ }
    
    //This constructor is just for testing. Use the above one for implementation
   // Item(time_t st, time_t et, accumulator_table_map_type d, int x)
   // :startTime(st), endTime(et), data(d), sum(x){}

    accumulator_table_map_type getData();
    time_t getStartTime();
    time_t getEndTime();
    void operator+=(Item &other);
    void print();
    void print_accumulator(std::string& accumulator_name, metric::metric_type feature);

    void setStartTime(time_t _st);
    void setEndTime(time_t _st);
    void clear();
    //boost::property_tree::ptree Jsonprint();
   // int getSum();

};

class Level
{
    boost::circular_buffer<Item*> itemsList;
    int noOfItemsToRollup;  // No.of items to rollup
    int rollupItemsAval;   // Number of items available for Rollup
    int size; // Total number of items stored in itemsList.
public:
    Level() {}
    Level(int s, int n);
    void levelPrint(void);
    //boost::property_tree::ptree levelPrintJson();
    int addItem(Item *itm);
    Item* mergeItems();
    int findQuery(time_t t, Item *result);
    int findRangeQuery(time_t *st, time_t *et, Item *result, bool bup);
    int getSize() {return size;}
    int get_noOfItemsToRollup();
    int get_bufferCapacity();
    boost::circular_buffer<Item*>& get_itemsList();
};

class RollupData {
    int noOfLevels;
    std::list<Level*> levelsList;
public:    
  /*
   RollupData(int n) : 
        levelsList(),
        noOfLevels(n)
    {
    } */
    RollupData();
    void setNoOfLevels(int _noOfLevels);
    int getNoOfLevels();
    void addLevel(Level* l);
    //void printJson_all(std::string json_file_name);
   // void printJson_item(Item *itm, std::string json_file_name);
    void printLevels();
    void addRollupItem(Item *itm);
    //int printJson_range(time_t st, time_t et, std::string json_file_name);
   // int printJson_range_BottomUp(time_t st, time_t et, std::string json_file_name);
    int pointQuery(time_t t, Item *result);
    
    // This does the search top to down. Search starts from rolled up levels to most
    // granular level
    int rangeQuery(time_t st, time_t et, Item *result);
    
    //This does the same rangeQuery reverse.
    int rangeQueryBottomUp(time_t st, time_t et, Item *result);
    
    std::list<Level*>& get_levelsList();
};

#endif
