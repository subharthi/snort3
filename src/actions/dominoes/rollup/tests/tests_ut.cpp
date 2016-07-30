#include "rollup.h"
#include "statistics.hpp"
#include <extractors/basic.hpp>
#include <random>
int main()
{
    
    using namespace extractors;
    int count = 100;
    
    RollupData rollup;
    rollup.setNoOfLevels(2);

    //RollupData rollup(2);

    Level level1(10,5);
    Level level2(10,6);
    //rollup.addLevel(level1);
    //rollup.addLevel(level2);
    rollup.addLevel(&level1);
    rollup.addLevel(&level2);
    Statistics statistics;
    time_t ft;
    statistics.add("total_count", {}, {}, {metric::count});
    statistics.add("count_per_srcip", {packet::src_ip}, {}, {metric::count});
    //statistics.add("count_per_srcip", {packet::src_ip}, {}, {metric::median});
    statistics.add("all sources to target dstip", {packet::dst_ip}, {packet::src_ip}, {metric::count});
    
    Packet fake_pkt1( 1, 10 );
    Packet fake_pkt2( 2, 10 );
    Packet fake_pkt3( 3, 11 );

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 3);
    
    time_t st = ft= time(NULL);
    //time_t et;
    for(int i=1;i<=24;i++)
    {
        for( int j = 0; j < count; ++j ) {
             int random = dis(gen);
             if (random == 1) {
                statistics(fake_pkt1);
             }
             else if (random ==2) {
                 statistics(fake_pkt2);
             }
             else {
                 statistics(fake_pkt3);
             }
        }        
        Item *itm = new Item(st,st+5,statistics.dump());
        rollup.printJson_item(itm, "tick_level.json");
        rollup.addRollupItem(itm);
        st+=5;
    }

    
    printf("Printing Levels\n");
    rollup.printLevels();
    Item *x = new Item();
    printf("Searching for time:%ld\n", ft+20);
    rollup.query(ft+20, x);
    free(x);
    x = new Item();
    printf("Searching for time:%ld, %ld\n", ft+60, ft+140);
    rollup.query(ft+60, ft+140, x);
    free(x);
    x = new Item();
    printf("Searching for time:%ld, %ld\n", ft+20, ft+40);
    rollup.query(ft+20, ft+40, x);
    free(x);
    
/*    rollup.printLevels();
      Item *x = new Item();
      printf("Searching for time:%ld\n", ft+20);
      rollup.pointQuery(ft+20, x);
      printf("Searching for time:%ld, %ld\n", ft+60, ft+140);
      rollup.rangeQuery(ft+60, ft+140, x);
      printf("Searching for time:%ld, %ld\n", ft+20, ft+40);
      rollup.rangeQuery(ft+20, ft+40, x); */
}
