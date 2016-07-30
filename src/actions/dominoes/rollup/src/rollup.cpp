#include "rollup.h"
#include "actions/dominoes/dominoes_detector_api.h"
#include "actions/dominoes/accumulators/src/accumulator_group.hpp"

/* Helper Functions */
static void printResult(std::list<Item*> result)
{
    std::list<Item*>::iterator rit;
    std::cout << "Final Result Entries" << std::endl;
    for (rit=result.begin(); rit!=result.end(); ++rit)
    {
        std::cout << "ST:" << (*rit)->getStartTime() << ", ET:" <<
                              (*rit)->getEndTime();

#ifdef ROLLUP_DEBUG
        std::cout<< ", Sum:"<<  (*rit)->getSum() <<std::endl;
#endif
    }
}

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
Item::Item():startTime(0), endTime(0), sum(0)
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
#ifdef ROLLUP_DEBUG
                std::cout << "Found Value:" ;
                itemsList[i]->print();
#endif
                return 0;
            }
        }
    }
    return -1;
}

int Level::query(time_t st, time_t et, std::list<Item*> &result)
{
    
    if (itemsList.empty()) {
        return 1;
    }

    Item *itmFront = itemsList.front();
    Item *itmBack = itemsList.back();

    if (result.empty())
    {
        //So far didnt find anything in the level above
        //Check this level and add them
        if ((st < itmFront->getStartTime() && et < itmFront->getStartTime() ) ||
                (st > itmBack->getEndTime() && et > itmBack->getEndTime()))
        {
            //Its not in this level at all
            return 0;
        } else {
            for (int i=0; i<itemsList.size(); i++)
            {
                // Full cell match
                if (itemsList[i]->getStartTime()>= st &&  itemsList[i]->getEndTime() <= et ||
                // Partial cell matches.
                        ((itemsList[i]->getStartTime() < st)&& (st < itemsList[i]->getEndTime())) ||
                        ((itemsList[i]->getStartTime() < et)&& (et < itemsList[i]->getEndTime())))
                {
#ifdef ROLLUP_DEBUG
                    std::cout << "Adding:"<< std::endl;
                    itemsList[i]->print();
#endif
                    result.push_back(itemsList[i]);
                }
            }
        }
    } else {
        // Something has been found in the previous level
        time_t resFST = result.front()->getStartTime();
        time_t resFET = result.front()->getEndTime();

        if (resFST != st)
        {
            Item *temp = new Item();
            query(st, temp);

            if (temp->getStartTime()!= 0)
            {
                //This level also has the start time
                if (temp->getStartTime() > resFST)
                {
                    //Lets optimize the start time
                    bool first = true;
                    std::list<Item*>::iterator it;
                    for (int i=0; i<itemsList.size(); i++)
                    {
                        if (itemsList[i]->getStartTime() >= temp->getStartTime() && itemsList[i]->getEndTime() <= resFET)
                        {
                            if (first)
                            {
#ifdef ROLLUP_DEBUG
                                std::cout << "Removing:"<< std::endl;
                                result.front()->print();
#endif
                                result.pop_front();
#ifdef ROLLUP_DEBUG
                                std::cout << "Adding:" << std::endl;
                                itemsList[i]->print();
#endif
                                result.push_front(itemsList[i]);
                                first = false;
                                it = result.begin();
                            } else {
#ifdef ROLLUP_DEBUG
                                std::cout << "Adding:" << std::endl;
                                itemsList[i]->print();
#endif
                                result.insert(it, itemsList[i]);
                            }
                            ++it;
                        }
                    }
                }
            }
        }

        time_t resBST = result.back()->getStartTime();
        time_t resBET = result.back()->getEndTime();
        if (resBET < et)
        {
            //Adding more here. Nothing to optimize
            addRange(resBET+1, et, result);
        } else {
            //Optimize end time if we can

            // If we can find the last cell start and end times in this level.
            // update that with more granular data
            if (isPresent(resBST))
            {
#ifdef ROLLUP_DEBUG
                std::cout << "Removing:" << std::endl;
                result.back()->print();
#endif
                result.pop_back();
                addRange(resBST, et, result);
            }
        }
    }
    return 0;
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

bool Level::isPresent(time_t ft)
{
    Item *itmFront = itemsList.front();
    Item *itmBack = itemsList.back();

    if (ft >= itmFront->getStartTime() &&
        ft <= itmBack->getEndTime())
        return true;

    return false;
}

void Level::addRange(time_t st, time_t et, std::list<Item *> &result)
{
    for (int i=0; i<itemsList.size(); i++)
    {
        if (itemsList[i]->getStartTime() >= st && itemsList[i]->getStartTime() <= et)
        {
#ifdef ROLLUP_DEBUG
            std::cout << "Adding:" << std::endl;
            itemsList[i]->print();
#endif
            result.push_back(itemsList[i]);
        }
    }
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

int RollupData::query(time_t st, time_t et, Item *res)
{
    std::list<Item*> result;
    for (std::list<Level*>::reverse_iterator it = levelsList.rbegin(); it != levelsList.rend(); it++)
    {
        (*it)->query(st, et, result);
    }
#ifdef ROLLUP_DEBUG
        std::cout << "Result Entries" << std::endl;
        printResult(result);
#endif

    std::list<Item*>::iterator rit;
    for (rit=result.begin(); rit!=result.end(); ++rit)
    {
        *res += *(*rit);
    }
    std::cout << "Final Item" << std::endl;
    res->print();
    return 0;
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
