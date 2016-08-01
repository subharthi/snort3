#ifndef ROLLUP_H
#define ROLLUP_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <list>
#include <string>
#include <fstream>
#include <ios>
#include <boost/circular_buffer.hpp>
#include "actions/dominoes/accumulators/src/statistics.hpp"

#define ROLLUP_DEBUG
/*  Gist for this file.
    Each Item contains accumulator Data
    Each Level contains 'n' number of Items 
    Each RollupData contains 'x' number of Levels */ 

/* Each instance of this class contains data for a certain period
   (which is identified by Start and End time) */
class Item
{
    time_t startTime;
    time_t endTime;
    accumulator_table_map_type  data;
#ifdef ROLLUP_DEBUG
    int sum;
#endif

public:
/* Constructors */
#ifdef ROLLUP_DEBUG
    Item();
    Item(time_t st, time_t et, accumulator_table_map_type d, int x);
#else
    Item();
#endif
    Item(time_t st, time_t et, accumulator_table_map_type d);

/* Getter and Setter Methods */    
    time_t getStartTime();
    time_t getEndTime();
    accumulator_table_map_type getData();
    void setStartTime(time_t _st);
    void setEndTime(time_t _st);
#ifdef ROLLUP_DEBUG
    int getSum();
#endif

/* Print Methods */
    void print();
    void print_accumulator(std::string& accumulator_name, metric::metric_type feature);

/* Overloaded Operators */    
    void operator+=(Item &other);

/* Others */
    void clear();
};

/* Each Level is a collection of n number of Items */
class Level
{
    boost::circular_buffer<Item*> itemsList;
    /* No.of items to rollup */
    int noOfItemsToRollup;
    /* Number of items available for Rollup */
    int rollupItemsAval;
    /* Total number of items stored in itemsList. */
    int size;

public:
    /* Constructors */
    Level();
    Level(int s, int n);

    /* Getter and Setter Methods */
    int getSize();
    int getNoOfItemsToRollup();
    int getBufferCapacity();
    boost::circular_buffer<Item*>& getItemsList();
   
    /* Print Methods */ 
    void print(void);

    /* Query Api's */
    int query(time_t t, Item *result);
    int query(time_t st, time_t et, std::list<Item*> &result);

    /* Others */
    int addItem(Item *itm);
    Item* mergeItems();
    bool isPresent(time_t);
    void addRange(time_t st, time_t et, std::list<Item*> &result);
};

/* This class is used to collect all the rollup data*/
class RollupData {
    int noOfLevels;
    std::list<Level*> levelsList;

public:    
    /* Constructors */
    RollupData();

    /* Getter and Setter Methods */
    void setNoOfLevels(int _noOfLevels);
    int getNoOfLevels();
    std::list<Level*>& getLevelsList();

    /* Print Methods */
    void print();

    /* Query Api's */
    int query(time_t t, Item *result);
    int query(time_t st, time_t et, Item *result);
    int query(time_t st, time_t et, std::list<Item *> &result);

    /* Others */
    void addLevel(Level* l);
    void addItem(Item *itm);
};

#endif
