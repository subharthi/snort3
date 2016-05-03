#ifndef __BUILDING_BLOCKS_HPP__
#define __BUILDING_BLOCKS_HPP__

#include "src/packet_extractor.hpp"


struct byte_offset : public packet_data_args {
    byte_offset(int o) : offset(o) {}

    uint64_t operator()(const Packet &value) const {
        // TODO take the specific offset of the raw data
        //return *(reinterpret_cast<const uint8_t*>((&value)) + offset);
        return offset;
    }

    std::ostream& print(std::ostream& out) const {
        out << "byte_offset(offset=" << offset << ")";
    }

private:
    const int offset;
};

struct Inspector : public packet_data_args {
    Inspector(PacketInspector i) : inspector(i) {}

    uint64_t operator()(const Packet &value) const {
        return ((value).*(inspector))();
    }

    std::ostream& print(std::ostream& out) const {
        out << "Inspector()";
    }

private:
    PacketInspector inspector;
};

#endif
