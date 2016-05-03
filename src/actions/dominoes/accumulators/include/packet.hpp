
struct Packet {
    Packet(int source_ip_) :
        ip_src(source_ip_),
        ip_verhl(1), ip_tos(2), ip_len(3), 
        ip_id(4), ip_off(6), ip_ttl(7), 
        ip_proto(8), ip_csum(9), ip_dst(10) { }

    uint8_t ip_verhl;      /* version & header length */
    uint8_t ip_tos;        /* type of service */
    uint16_t ip_len;       /* datagram length */
    uint16_t ip_id;        /* identification  */
    uint16_t ip_off;       /* fragment offset */
    uint8_t ip_ttl;        /* time to live field */
    uint8_t ip_proto;      /* datagram protocol */
    uint16_t ip_csum;      /* checksum */
    uint32_t ip_src;       /* source IP */
    uint32_t ip_dst;       /* dest IP */

    /* getters */
    inline int hlen() const
    { return (uint8_t)((ip_verhl & 0x0f) << 2); }

    inline int ver() const
    { return (ip_verhl >> 4); }

    inline int tos() const
    { return ip_tos; }

    inline int len() const
    { return ip_len; }

    inline int ttl() const
    { return ip_ttl; }

    inline int proto() const
    { return ip_proto; }

    inline int get_src() const
    { return ip_src; }

    inline int get_dst() const
    { return ip_dst; }

};

typedef int  (Packet::*PacketInspector)() const;
