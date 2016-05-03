#ifndef __BASIC_HPP__
#define __BASIC_HPP__

#include "extractors/building_blocks.hpp"

namespace extractors { 
    
namespace packet {

//struct header_len : public packet_data_args {
//    int operator()(const Packet &value) const {
//        return value.hlen();
//    }
//};

const Inspector hlen(&Packet::hlen);
const Inspector ver(&Packet::ver);
const Inspector tos(&Packet::tos);
const Inspector len(&Packet::len);
const Inspector ttl(&Packet::ttl);
const Inspector proto(&Packet::proto);
const Inspector src_ip(&Packet::get_src);
const Inspector dst_ip(&Packet::get_dst);

namespace tcp {
//const Inspector src_port();
//const Inspector dst_port();
//const Inspector len();
}

namespace udp {
//const Inspector src_port();
//const Inspector dst_port();
//const Inspector len();
}

namespace icmp {

}

} 

}

#endif
