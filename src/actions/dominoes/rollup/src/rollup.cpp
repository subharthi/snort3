#include "rollup.h"
#include "actions/dominoes/dominoes_detector_api.h"
#include "actions/dominoes/accumulators/src/accumulator_group.hpp"


/* Constructors */
#ifndef ROLLUP_DEBUG
Item::Item():startTime(0), endTime(0)
{

}
#endif

Item::Item(time_t st, time_t et, accumulator_table_map_type d)
:startTime(st), endTime(et), data(d)
{ 

}

#ifdef ROLLUP_DEBUG
Item():startTime(0), endTime(0), sum(0)
{

}

Item::Item(time_t st, time_t et, accumulator_table_map_type d, int s)
:startTime(st), endTime(et), data(d), sum(s)
{

}
#endif

/* Getter and Setter Methods */
time_t Item::getStartTime()
{
    return startTime;
}

time_t Item::getEndTime()
{
    return endTime;
}

accumulator_table_map_type Item::getData()
{
    accumulator_table_map_type temp = data;
    return temp;
}

#ifdef ROLLUP_DEBUG
int Item::getSum()
{
    return sum;
}
#endif

void Item::setStartTime(time_t st)
{
    startTime = st;
}

void Item::setEndTime(time_t et)
{
    endTime = et;
}

/* Print Methods */
void Item::print()
{
    std::cout << startTime << " : " << endTime << " : ";
    for ( auto &i : data) {
        std::cout<< i.first << ":";
        for( auto &k : i.second.data ) {
            std::cout << extract_result<tag::count>(k.second) << ":";
        }

        std::cout << std::endl;
    }
#ifdef ROLLUP_DEBUG
    std::cout << sum << std::endl;
#endif
}

void Item::print_accumulator(std::string& accumulator_name, metric::metric_type feature)
{
    std::cout << accumulator_name << ":" << startTime << " : " << endTime << " : " << std::endl;
    for ( auto &i : data) {
        if(std::string(i.first) == accumulator_name){
            for( auto &k : i.second.data ) {
                std::cout  << k.first << ":" << extract_result(feature, k.second) << std::endl;
            }
            std::cout << std::endl;
            return;
        }
    }
}

/* Overloaded Operators */
void Item::operator+=(Item &other) {
    // make the start time the former of the two
    if (startTime != 0)
    {
        if( startTime > other.startTime ) {
            startTime = other.startTime;
        }
    } else 
    {
        startTime = other.startTime;
    }

    // make the end time the later of the two
    if( endTime != 0)
    {
        if( endTime < other.endTime ) {
            endTime = other.endTime;
        }
    } else 
    {
        endTime = other.endTime;
    }
    //data += other.data;
    for( auto &i : other.data ) {
        auto iter = data.find(i.first);
        if( iter == data.end() ) {
            data.insert(i);
            continue;
        } else {
            iter->second += i.second;
        }

    }

#ifdef ROLLUP_DEBUG
    sum +=other.sum;
#endif
}

/* Others */
void Item::clear()
{
    for (auto &i : data) {
        i.second.data.clear();
    }
}

/* Level class member functions */

/* Constructors */
Level::Level()
{

}

Level::Level(int s, int n):itemsList(s)
{
    size = s;
    noOfItemsToRollup = n;
    rollupItemsAval = 0;
}

/* Getter and Setter Methods */ 
int Level::getSize() 
{
    return size;
}

int Level::getNoOfItemsToRollup()
{
    return noOfItemsToRollup;
}

int Level::getBufferCapacity() 
{
    return itemsList.capacity();
}

boost::circular_buffer<Item*>& Level::getItemsList()
{ 
    return itemsList; 
} 

/* Print Methods */
void Level::print(void)
{
    int i= size;
    for (i=0; i< itemsList.size(); i++)
    {
        std::cout << "---------------------------------------------" << std::endl;
        itemsList[i]->print();
    }
}

/* Query Api's */
int Level::query(time_t t, Item *result)
{
    Item *itmFront = itemsList.front();
    Item *itmBack = itemsList.back();
    if (t >= itmFront->getStartTime() && t <= itmBack->getEndTime())
    {
        //Found in this level
        for (int i=0; i< itemsList.size(); i++)
        {
            if (t >= itemsList[i]->getStartTime() && t <= itemsList[i]->getEndTime() )
            {
                *result += *itemsList[i];
                //printf("Test:Found Value:%d\n",itemsList[i].getSum());
                return 0;
            }
        }
    }
    return -1;
}

int Level::rangeQuery(time_t *st, time_t *et, Item *result)
{

    if (itemsList.empty()){
        return 1;
    }

    Item* itmFront = itemsList.front();
    Item* itmBack = itemsList.back();

    // Complete range can be found in this level
    // For now include as soon as the startTime and endTime match
    // For cases like half match of the cell 
    // (ex: if cell has 1 to 2 pm and range query starts from 1.30 to 2.30)
    // include both the cells

    if( *st >= itmFront->getStartTime() && *et <= itmBack->getEndTime())
    {       
        for (int i=0; i< itemsList.size(); i++)
        {
            // Full cell match
            if (itemsList[i]->getStartTime()>= *st &&  itemsList[i]->getEndTime() <= *et ||
                    // Partial cell matches.
                    ((itemsList[i]->getStartTime() < *st)&& (*st < itemsList[i]->getEndTime())) ||
                    //      itemsList[i].getStartTime() < et < itemsList[i].getEndTime())
                ((itemsList[i]->getStartTime() < *et)&& (*et < itemsList[i]->getEndTime())))
                {
                    //TODO:Add the item
                    *result += *itemsList[i];
                } else if (itemsList[i]->getEndTime() > *et)
                {
                    // This is for optimization
                    // Found all the cells. just break
                    break;
                }
        }
        return 0;
    } else if ((*st < itmFront->getStartTime() && *et < itmFront->getStartTime() ) ||
            (*st > itmBack->getEndTime() && *et > itmBack->getEndTime()))
    {
        //Its not in this level at all
        return 1;
    }
    else
    {
        // Only few elements are found in this level. 
        // Few more items will be found in the level below with more recent data.
        for (int i=0; i< itemsList.size(); i++)
        {
            // Full cell match
            if (itemsList[i]->getStartTime()>= *st &&  itemsList[i]->getEndTime() <= *et ||
                    ((itemsList[i]->getStartTime() < *st)&& (*st <= itemsList[i]->getEndTime())) ||
                    ((itemsList[i]->getStartTime() < *et)&& (*et < itemsList[i]->getEndTime())))
            {
                //TODO:Add the item
                // std::cout << "Adding Item 1:" << itemsList[i]->getSum()<< std::endl;
                *result += *itemsList[i];
                *st  = itemsList[i]->getEndTime()+1;
            }
        }
        return 1;
    } 
}

/* Others */
int Level::addItem(Item *itm)
{
    int ret = 0;
    itemsList.push_back(itm);

    rollupItemsAval++;
    if (rollupItemsAval == noOfItemsToRollup)
    {
        ret = 1;
        rollupItemsAval = 0;
    }
    return ret;
}

Item* Level::mergeItems()
{
    Item *itm = new Item();

    boost::circular_buffer<Item*>::reverse_iterator it=itemsList.rbegin();
    for(int i=0; i< noOfItemsToRollup; ++it, ++i)
    {
        *itm += *(*it);
    }
    return itm;
}

/* RollupdData class member functions */

/* Constructors */
RollupData::RollupData():levelsList()
{}

/* Getter and Setter Methods */
int RollupData::getNoOfLevels()
{
    return noOfLevels;
}

std::list<Level*>& RollupData::getLevelsList()
{
    return levelsList;
}

void RollupData::setNoOfLevels(int _noOfLevels) 
{
    noOfLevels = _noOfLevels;
}

/* Print Methods */
void RollupData::print()
{
    int i=1;
    for ( auto &it : levelsList )
    {
        std::cout << "---Level " << i << "----" << std::endl;
        ++i;
        it->print();
    }
}

/* Query Api's */
int RollupData::query(time_t t, Item *result)
{
    int ret = -1;
    //For the  point query, search happens bottom up. 
    //We start searching from most granular level to higher rollup levels
    for(auto &it:levelsList)
    {
        ret = it->query(t, result);
        if (ret == 0)
        {
            return 0;  
        }
    }
    return -1;
}

int RollupData::rangeQuery(time_t st, time_t et, Item *result)
{
    int ret = -1; 

    for (std::list<Level*>::reverse_iterator it = levelsList.rbegin(); it != levelsList.rend(); it++)
    {
        ret = (*it)->rangeQuery(&st, &et, result);
        if (ret == 0)
            return 0;
    }

    return -1;
}

/* Others */
void RollupData::addLevel(Level* l)
{
    levelsList.push_back(l);
}

void RollupData::addItem(Item *itm)
{
    int ret=0;
    Item *temp = itm;

    for (auto& it : levelsList )
    {
        ret = it->addItem(temp);
        if(ret == 0)
            break;
        temp = it->mergeItems();
    }
}
