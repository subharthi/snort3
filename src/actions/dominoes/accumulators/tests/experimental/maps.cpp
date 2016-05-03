#include <stdlib.h>
#include <iostream>
#include <map>

using namespace std;

typedef map<string, int> int_map;
typedef map<string, int_map> mega_map;

// The last parameter is just setting the out stream to be std::cout by default
ostream &print_mega_map(const mega_map &mm, std::ostream &out = cout) {

    // Print the name so the output is easier to read
    out << "{" << std::endl;

    for( auto &i : mm ) {
        out << "  " << i.first << " : {" << std::endl;
        for( auto &k : i.second ) {
            out << "    { " << k.first << " : ";
            out << k.second << " }" << std::endl;
        }
        out << "  }" << std::endl;
    }
    out << "}" << std::endl;

    return out;
}

mega_map &merge_mega_map( mega_map &left, const mega_map &right ) {

    // iterate over each key in the right map checking if it is in the left map
    // if it is merge at the 2nd map level
    // if not then just instert that entire key/value pair
    for( auto &i : right ) {
        auto mega_it = left.find(i.first); // figure out if/where the key is
        if( mega_it == left.end() ) { // if it doesn't exist just copy the map
            left.insert(i);
            continue;
        }
        // the key does exist so now we move onto the next level of maps
        for( auto &k : i.second ) {
            auto int_it = mega_it->second.find(k.first); // figure out if/where the key is
            if( int_it == mega_it->second.end() ) { 
                // if it doesn't exist just copy the string int pair
                mega_it->second.insert(k);
                continue;
            }
            // we have some overlap and merging of the actual elements needs to happen
            int_it->second += k.second;
        }
    }

    return left;
}

// These two functions are just to show off operator overloading
mega_map &operator+=(mega_map &left, const mega_map &right) {
    return merge_mega_map(left, right);
}

ostream &operator<<(ostream &out, const mega_map &value) {
    return print_mega_map(value, out);
}


/*
 * This shows creating several "mega maps" and merging them.
 */
int main() {

    // Create mega map 1
    std::cout << "mm1 : ";
    mega_map mm1{};

    mm1["a"]["a"] = 1;

    print_mega_map(mm1);


    // Create mega map 2
    std::cout << "mm2 : ";
    mega_map mm2{};

    // a non overlapping top level key
    mm2["b"]["a"] = 1;

    print_mega_map(mm2);


    // Create mega map 3
    std::cout << "mm3 : ";
    mega_map mm3{};
    // overlapping top level key, non overlapping second level
    mm3["a"]["b"] = 1;

    print_mega_map(mm3);

    // Create mega map 4
    std::cout << "mm4 : ";
    mega_map mm4{};
    // overlapping top level key, overlapping second level
    mm4["a"]["a"] = 1;

    print_mega_map(mm4);

    // Start the merging
    std::cout << std::endl;
    std::cout << "non overlapping top level merge" << std::endl;
    merge_mega_map(mm1, mm2);
    std::cout << "merged : ";
    print_mega_map(mm1);

    std::cout << "overlapping top level, non overlapping second level merge" << std::endl;
    merge_mega_map(mm1, mm3);
    std::cout << "merged : ";
    print_mega_map(mm1);

    std::cout << "overlapping top level, overlapping second level merge" << std::endl;
    merge_mega_map(mm1, mm4);
    std::cout << "merged : ";
    print_mega_map(mm1);


    // Showing off how nice operator overloading can make things look
    // this just merged mm4 with mm1 a different way
    std::cout << "overloaded" << std::endl;
    mm1 += mm4;
    std::cout << "merged : " << mm1;
}




