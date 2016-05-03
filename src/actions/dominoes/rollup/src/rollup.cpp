#include "rollup.h"
#include "actions/dominoes/dominoes_detector_api.h"
#include "actions/dominoes/accumulators/src/accumulator_group.hpp"

//using boost::property_tree::ptree;

Item::Item(time_t st, time_t et, accumulator_table_map_type d)
    :startTime(st), endTime(et), data(d)
{ 

}

accumulator_table_map_type Item::getData()
{
    accumulator_table_map_type temp = data;
    return temp;
}
time_t Item::getStartTime()
{
    return startTime;
}
time_t Item::getEndTime()
{
    return endTime;
}
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
    //sum +=other.sum;
}

void Item::setStartTime(time_t st)
{
	startTime = st;
}
void Item::setEndTime(time_t et)
{
        endTime = et;
}

void Item::clear()
{
	for (auto &i : data) {
		i.second.data.clear();
        }
}
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
    //printf("Sum:%d\n", sum);
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
/*
boost::property_tree::ptree Item::Jsonprint()
{
    ptree child;
    ptree children_of_child, children_of_child2, children_of_child3, cfc3, cfc4;

    children_of_child.put("Start Time", startTime);
    children_of_child.put("End Time", endTime);
    for ( auto &i : data) {
        ptree cfc2;
        for( auto &k : i.second.data ) {
                ptree cfc1;
                cfc1.put("count", extract_result<tag::count>(k.second));
                cfc2.push_back(std::make_pair(std::string(k.first), cfc1));
                //cfc3.put_child("Keys", cfc2);  
                //cfc1.put("metric", k.second); FIX k.second should extract the metric
                //cfc3.add_child(std::string(i.first), cfc4);
        }
        cfc3.push_back(std::make_pair(std::string(i.first), cfc2));
        children_of_child3.put_child(std::string("statistics_list"), cfc3);
    }
    children_of_child2.add_child(std::string("syn-observer"), children_of_child3);
    children_of_child.add_child(std::string("observations"), children_of_child2);
    child.add_child(std::string("syn-attack-detector"), children_of_child);
    return child;
}
*/

Level::Level(int s, int n) :itemsList(s)
{
        size = s;
        noOfItemsToRollup = n;
        rollupItemsAval = 0;
}

void Level::levelPrint(void)
{
    int i= size;
    for (i=0; i< itemsList.size(); i++)
    {
        printf("---------------------------------------------\n");
        itemsList[i]->print();
    }

}

/*
boost::property_tree::ptree Level::levelPrintJson()
{
    int i= size;
    ptree child;
    ptree children;
    for (i=0; i< itemsList.size(); i++)
    {
        child=itemsList[i]->Jsonprint();
        children.push_back(std::make_pair("", child));
    }
    return children;
}
*/
int Level::get_noOfItemsToRollup()
{
	return noOfItemsToRollup;
}

int Level::get_bufferCapacity() 
{
	return itemsList.capacity();
}

boost::circular_buffer<Item*>& Level::get_itemsList()
{ 
	return itemsList; 
} 

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
int Level::findQuery(time_t t, Item *result)
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

int Level::findRangeQuery(time_t *st, time_t *et, Item *result, bool bottomUp)
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
                    if (!bottomUp)
                    {
                        *st  = itemsList[i]->getEndTime()+1;
                    }               
                }
        }
        if (bottomUp)
        {
            *et = result->getStartTime()-1;
        }
        return 1;
    } 
}
/*
int Level::findRangeQuery(time_t *st, time_t *et, Item *result)
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
                    //printf("Adding item:%d\n", itemsList[i].getSum());  
                    *result += *itemsList[i];
                } else if (itemsList[i]->getEndTime() > *et)
                {
                    // This is for optimization
                    // Found all the cells. just break
                    //printf("Value found:%d", *result);
                    break;
                }
        }
        return 0;
    } else
    {
        // Only few elements are found in this level. 
        // Few more items will be found in the level below with more recent data.

        for (int i=0; i< itemsList.size(); i++)
        {
            // Full cell match
            if (itemsList[i]->getStartTime()>= *st &&  itemsList[i]->getEndTime() <= *et ||
                    ((itemsList[i]->getStartTime() < *st)&& (*st <= itemsList[i]->getEndTime())) ||
                    //      itemsList[i].getStartTime() < et < itemsList[i].getEndTime())
                ((itemsList[i]->getStartTime() < *et)&& (*et < itemsList[i]->getEndTime())))
                {
                    //TODO:Add the item
                    //printf("Adding item 1:%d\n",itemsList[i].getSum());
                    *result += *itemsList[i];
                    *st  = itemsList[i]->getEndTime()+1;               
                }
        }
        // Now we need to collect more cells in the next level.
        // Update starttime we are finding to endtime of this level.
        //st = itmBack.getEndTime();
        return 1;

    } 
}
*/
RollupData::RollupData():levelsList()
{}

int RollupData::getNoOfLevels()
{
    return noOfLevels;
}

void RollupData::setNoOfLevels(int _noOfLevels) 
{
	noOfLevels = _noOfLevels;
}

std::list<Level*>& RollupData::get_levelsList()
{
	return levelsList;
}

void RollupData::addLevel(Level* l)
{
    levelsList.push_back(l);
}
/*
void RollupData::printJson_item(Item *itm, std::string json_file_name)
{
    Item *temp = itm;
    std::ofstream outfile (json_file_name, std::ios_base::app);
    ptree root;
    root=temp->Jsonprint();
    write_json(outfile, root);
}
void RollupData::printJson_all(std::string json_file_name)
{
    std::ofstream outfile (json_file_name);
    int i=1;
    ptree root;
    ptree children;
    for ( auto &it : levelsList )
    {
        std::string s = "level" + std::to_string(i);
        ++i;
        children=it->levelPrintJson();
        root.add_child(s, children);
    }
    write_json(outfile, root);
}*/
void RollupData::printLevels()
{
    int i=1;
    for ( auto &it : levelsList )
    {
        std::cout << "---Level " << i << "----" << std::endl;
        ++i;
        it->levelPrint();
    }
}
void RollupData::addRollupItem(Item *itm)
{
    int ret=0;
    Item *temp = itm;
    //Item* local_item = new Item(itm->getStartTime(), itm->getEndTime(), itm-> getData());
    for (auto& it : levelsList )
    {
        ret = it->addItem(temp);	
	if(ret == 0)
            break;
	//std::cout<< "merging items"<< std::endl;
        temp = it->mergeItems();
    }
}
/*
int RollupData::printJson_range(time_t st, time_t et, std::string json_file_name)
{
    Item *temp = new Item();
    std::ofstream outfile (json_file_name);
    int ret = -1;
    for (std::list<Level*>::iterator it = levelsList.begin(); it != levelsList.end(); it++)
    {
        ret = (*it)->findRangeQuery(&st, &et, temp, 0);
        if (ret == 0)
        {
           ptree root;
           root=temp->Jsonprint();
           write_json(outfile, root);
           free(temp);
           return 0;
        }
    }
    free(temp);
    return -1;
}

int RollupData::printJson_range_BottomUp(time_t st, time_t et, std::string json_file_name)
{
    Item *temp = new Item();
    std::ofstream outfile (json_file_name);
    int ret = -1;
    for (std::list<Level*>::iterator it = levelsList.begin(); it != levelsList.end(); it++)
    {
        ret = (*it)->findRangeQuery(&st, &et, temp, 1);
        if (ret == 0)
        {
           ptree root;
           root=temp->Jsonprint();
           write_json(outfile, root);
           free(temp);
           return 0;
        }
    }
    free(temp);
    return -1;
}
*/
int RollupData::pointQuery(time_t t, Item *result)
{
    int ret = -1;
    //For the  point query, search happens bottom up. 
    //We start searching from most granular level to higher rollup levels
    for(auto &it:levelsList)
    {
        ret = it->findQuery(t, result);
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
       // std::cout <<      printf("Searching level\n");
        ret = (*it)->findRangeQuery(&st, &et, result, 0);
        if (ret == 0)
            return 0;
    }

    return -1;
}

int RollupData::rangeQueryBottomUp(time_t st, time_t et, Item *result)
{
   int ret = -1;
   //std::cout << st << "::" << et << std::endl;
    for (std::list<Level*>::iterator it = levelsList.begin(); it != levelsList.end(); it++)
    {
        ret = (*it)->findRangeQuery(&st, &et, result, 1);
        if (ret == 0)
            return 0;
    }
    return -1;
}
