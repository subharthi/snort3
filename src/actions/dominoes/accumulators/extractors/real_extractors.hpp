#ifndef __REAL_EXTRACTORS_HPP__
#define __REAL_EXTRACTORS_HPP__
#include "src/packet_extractor.hpp"
#include "flow/flow.h"
#include <iostream>
namespace extractors {

//struct extractor : public packet_data_args { 
//    extractor(uint64_t offset) : 
//        byte_offset(offset),
//        name("byte_offset"),
//        description("returns the byte at the specified number of bytes") {}
//
//    uint64_t operator()(const Packet &packet) const {
//        return static_cast<uint8_t*>(packet)[byte_offset];
//    }
//private:
//    const uint64_t byte_offset;
//};

namespace packet {
namespace ipv4 {
enum class Field {
    hlen,
    ver,
    tos,
    len,
    ttl,
    proto,
    src_ip,
    dst_ip,
    client_ip,
    server_ip,
    client_port,
    server_port 
};
struct extractor : public packet_data_args { 
    extractor(Field f) : field(f) {}
     uint64_t operator()(const Packet &packet) const {
	if (!packet.is_ip4()){
		throw value_not_ready("ipv6 value found");
	}
        const ip::IP4Hdr *const ip4h = packet.ptrs.ip_api.get_ip4h();
	switch(field) {
	
            case Field::hlen:   return static_cast<uint64_t>(ip4h->hlen());
            case Field::ver:    return static_cast<uint64_t>(ip4h->ver());
            case Field::tos:    return static_cast<uint64_t>(ip4h->tos());
            case Field::len:    return static_cast<uint64_t>(ip4h->len());
            case Field::ttl:    return static_cast<uint64_t>(ip4h->ttl());
            case Field::proto:  return static_cast<uint64_t>(ip4h->proto());
            case Field::src_ip: return static_cast<uint64_t>(ip4h->get_src());
            case Field::dst_ip: return static_cast<uint64_t>(ip4h->get_dst());
	    // TODO: if flow data is private, need to add an api
	    // TODO: extract only if ipv4
	    case Field::client_ip:
					if (!packet.flow){
						//std::cout << "FLow not initialized" << std::endl;
						throw value_not_ready("packet flow structure not initialized");
					}	
					if (packet.flow->client_ip.is_ip4()){
						union {
							uint64_t ret;
							uint32_t temp[2];
						} const p_instance = { .temp = { packet.flow->client_ip.ip32[0],
								   	         packet.flow->client_ip.ip32[1]}
								     };
						return static_cast<uint64_t> (p_instance.ret);
					
					}
	    case Field::server_ip:	
					if (!packet.flow){
						throw value_not_ready("packet flow structure not initialized");
					}
					if (packet.flow->server_ip.is_ip4()){
                                                union {
                                                        uint64_t ret;
                                                        uint32_t temp[2];
                                                } const p_instance = { .temp = { packet.flow->server_ip.ip32[0],
                                                                                 packet.flow->server_ip.ip32[1]}
                                                                     };
                                                return static_cast<uint64_t> (p_instance.ret);	
                                        }

            case Field::client_port:    return static_cast<uint64_t> (packet.flow->client_port);
	    case Field::server_port: 	return static_cast<uint64_t> (packet.flow->server_port); 	 
//	    case Mapping::user_id: return get_usr_name(static_cast<uint64_t>(ip4h->get_dst()), ip_usr_map, usr_name);
        }
    }

   std::ostream& print(std::ostream& out) const {
        return out << "extractor(" << static_cast<uint64_t>(field) << ")";  
  }
   
  std::string stringify() const {
	switch(field){
	    case Field::hlen:   return std::string("header_len");
            case Field::ver:    return std::string("version");
            case Field::tos:    return std::string("type_of_service");
            case Field::len:    return std::string("num_bytes");
            case Field::ttl:    return std::string("time_to_live");
            case Field::proto:  return std::string("proto");
            case Field::src_ip: return std::string("src_ip");
            case Field::dst_ip: return std::string("dst_ip"); 
	    case Field::client_ip: return std::string("client_ip");
	    case Field::server_ip: return std::string("server_ip");
	    case Field::client_port: return std::string("client_port");
            case Field::server_port: return std::string("server_port");	
	}

  }
private:
    const Field field;


};
const extractor hlen(Field::hlen);
const extractor ver(Field::ver);
const extractor tos(Field::tos);
const extractor len(Field::len);
const extractor ttl(Field::ttl);
const extractor proto(Field::proto);
const extractor src_ip(Field::src_ip);
const extractor dst_ip(Field::dst_ip);
const extractor client_ip(Field::client_ip);
const extractor server_ip(Field::server_ip);
const extractor client_port(Field::client_port);
const extractor server_port(Field::server_port);




/* Added by Subharthi */
/*
namespace http {
enum class Field{
 	error,
	http_client_body,
	http_cookie,
	http_header,
	http_method,
	http_uri
		
};

struct extractor : public packet_data_args {
    extractor(Field f) : field(f) {}

    uint64_t operator()(const Packet &packet) const {
        const ip::IP4Hdr *const ip4h = packet.ptrs.ip_api.get_ip4h();
        switch(field) {
            case Field::hlen:   return static_cast<uint64_t>(ip4h->hlen());
            case Field::ver:    return static_cast<uint64_t>(ip4h->ver());
            case Field::tos:    return static_cast<uint64_t>(ip4h->tos());
            case Field::len:    return static_cast<uint64_t>(ip4h->len());
            case Field::ttl:    return static_cast<uint64_t>(ip4h->ttl());
            case Field::proto:  return static_cast<uint64_t>(ip4h->proto());
            case Field::src_ip: return static_cast<uint64_t>(ip4h->get_src());
            case Field::dst_ip: return static_cast<uint64_t>(ip4h->get_dst());
        }
    }

    std::ostream& print(std::ostream& out) const {
        return out << "extractor(" << static_cast<uint64_t>(field) << ")";
    }
private:
    const Field field;
};



} */
//const packet_data hlen {
//[](Packet &p) { return static_cast<uint64_t>(p.ptrs.ip_api.get_ip4h()->hlen()); },
//[](uint64_t &v) { return v; },
//"hlen",
//"header length"
//};
//
//const packet_data src_ip {
//[](Packet &p) { return static_cast<uint64_t>(p.ptrs.ip_api.get_ip4h()->get_src()); },
//[](uint64_t &v) { return v; },
//"src_ip",
//"packets source ip"
//};

namespace ipv6 {
// TODO write ipv6 extractors also
//enum Field {
//    len,
//    proto,
//    ver,
//    tos,
//    hlen,
//    src_ip,
//    dst_ip,
//
//};

//struct extractor : public packet_data_args { 
//    extractor(Field f) : field(f) {}
//
//    int operator()(const Packet &packet) const {
//        const ip::IP4Hdr *const ip6h = packet.ptrs.ip_api.get_ip6h();
//        switch(field) {
//        }
//    }
//private:
//    Field field;
//};
}
namespace tcp {
// TODO create tcp extractors
//const Inspector src_port();
//const Inspector dst_port();
//const Inspector len();
}

namespace udp {
#if 0
enum class Field {
    len,
    src_port,
    dst_port,
};

struct extractor : public packet_data_args { 
    extractor(Field f) : field(f) {}

    uint64_t operator()(const Packet &packet) const {
        const udp::UDPHdr *const uh = /* TODO */
        switch(field) {
            case Field::len:   return static_cast<uint64_t>(uh->len());
            case Field::src_port:    return static_cast<uint64_t>(uh->src_port());
            case Field::dst_port:    return static_cast<uint64_t>(uh->dst_port());
        }
    }
private:
    const Field field;
};

const extractor len(Field::len);
const extractor src_port(Field::src_port);
const extractor dst_port(Field::dst_port);
#endif
}
}
#if 0
namespace vlan {
struct vid_ : public packet_data_args { 
    uint64_t operator()(const Packet &packet) const {
        const vlan::VlanTagHdr *const vh = get_vlan_layer(packet);
        return static_cast<uint64_t>(vh->vid());
    }
};

const extractor len(Field::len);
const extractor src_port(Field::src_port);
const extractor dst_port(Field::dst_port);
#endif
}
}
#endif
