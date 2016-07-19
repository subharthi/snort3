//--------------------------------------------------------------------------
// Copyright (C) 2014-2016 Cisco and/or its affiliates. All rights reserved.
// Copyright (C) 2005-2013 Sourcefire, Inc.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------
// flow_key.cc author Steven Sturges <ssturges@sourcefire.com>

#include "flow/flow_key.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "main/snort_config.h"
#include "utils/util.h"
#include "sfip/sf_ip.h"
#include "protocols/packet.h"
#include "protocols/icmp4.h"
#include "protocols/icmp6.h"

//-------------------------------------------------------------------------
// init foo
//-------------------------------------------------------------------------

inline void FlowKey::init4(
    IpProtocol ip_proto,
    const sfip_t *srcIP, uint16_t srcPort,
    const sfip_t *dstIP, uint16_t dstPort,
    uint32_t mplsId, bool order)
{
    const uint32_t* src;
    const uint32_t* dst;

    if ( ip_proto ==  IpProtocol::ICMPV4 )
    {
        if (srcPort == ICMP_ECHOREPLY)
        {
            dstPort = ICMP_ECHO; /* Treat ICMP echo reply the same as request */
            srcPort = 0;
        }
        else /* otherwise, every ICMP type gets different key */
        {
            dstPort = 0;
        }
    }

    src = srcIP->ip32;
    dst = dstIP->ip32;

    /* These comparisons are done in this fashion for performance reasons */
    if ( !order || *src < *dst)
    {
        COPY4(ip_l, src);
        COPY4(ip_h, dst);
        port_l = srcPort;
        port_h = dstPort;
    }
    else if (*src == *dst)
    {
        COPY4(ip_l, src);
        COPY4(ip_h, dst);
        if (srcPort < dstPort)
        {
            port_l = srcPort;
            port_h = dstPort;
        }
        else
        {
            port_l = dstPort;
            port_h = srcPort;
        }
    }
    else
    {
        COPY4(ip_l, dst);
        port_l = dstPort;
        COPY4(ip_h, src);
        port_h = srcPort;
    }
    if (SnortConfig::mpls_overlapping_ip() &&
        ip::isPrivateIP(*src) && ip::isPrivateIP(*dst))
        mplsLabel = mplsId;
    else
        mplsLabel = 0;
}

inline void FlowKey::init6(
    IpProtocol ip_proto,
    const sfip_t *srcIP, uint16_t srcPort,
    const sfip_t *dstIP, uint16_t dstPort,
    uint32_t mplsId, bool order)
{
    const sfip_t* src;
    const sfip_t* dst;

    if ( ip_proto == IpProtocol::ICMPV4 )
    {
        if (srcPort == ICMP_ECHOREPLY)
        {
            /* Treat ICMP echo reply the same as request */
            dstPort = ICMP_ECHO;
            srcPort = 0;
        }
        else
        {
            /* otherwise, every ICMP type gets different key */
            dstPort = 0;
        }
    }
    else if ( ip_proto == IpProtocol::ICMPV6 )
    {
        if (srcPort == icmp::Icmp6Types::ECHO_REPLY)
        {
            /* Treat ICMPv6 echo reply the same as request */
            dstPort = icmp::Icmp6Types::ECHO_REQUEST;
            srcPort = 0;
        }
        else
        {
            /* otherwise, every ICMP type gets different key */
            dstPort = 0;
        }
    }

    src = srcIP;
    dst = dstIP;

    if ( !order || sfip_fast_lt6(src, dst))
    {
        COPY4(ip_l, src->ip32);
        port_l = srcPort;
        COPY4(ip_h, dst->ip32);
        port_h = dstPort;
    }
    else if (sfip_fast_eq6(src, dst))
    {
        COPY4(ip_l, src->ip32);
        COPY4(ip_h, dst->ip32);
        if (srcPort < dstPort)
        {
            port_l = srcPort;
            port_h = dstPort;
        }
        else
        {
            port_l = dstPort;
            port_h = srcPort;
        }
    }
    else
    {
        COPY4(ip_l, dst->ip32);
        port_l = dstPort;
        COPY4(ip_h, src->ip32);
        port_h = srcPort;
    }

    if (SnortConfig::mpls_overlapping_ip())
        mplsLabel = mplsId;
    else
        mplsLabel = 0;
}

void FlowKey::init_vlan(uint16_t vlanId)
{
    if (!SnortConfig::get_vlan_agnostic())
        vlan_tag = vlanId;
    else
        vlan_tag = 0;
}

void FlowKey::init_address_space(uint16_t addrSpaceId)
{
    if (!SnortConfig::address_space_agnostic())
        addressSpaceId = addrSpaceId;
    else
        addressSpaceId = 0;
    addressSpaceIdPad1 = 0;
}

void FlowKey::init_mpls(uint32_t mplsId)
{
    if (SnortConfig::mpls_overlapping_ip())
        mplsLabel = mplsId;
    else
        mplsLabel = 0;
}

void FlowKey::init(
    PktType type, IpProtocol ip_proto,
    const sfip_t *srcIP, uint16_t srcPort,
    const sfip_t *dstIP, uint16_t dstPort,
    uint16_t vlanId, uint32_t mplsId, uint16_t addrSpaceId)
{
    /* Because the key is going to be used for hash lookups,
     * the lower of the values of the IP address field is
     * stored in the ip_l and the port for that ip is
     * stored in port_l.
     */
    if (srcIP->is_ip4())
    {
        version = 4;
        init4(ip_proto, srcIP, srcPort, dstIP, dstPort, mplsId);
    }
    else
    {
        version = 6;
        init6(ip_proto, srcIP, srcPort, dstIP, dstPort, mplsId);
    }

    pkt_type = type;

    init_vlan(vlanId);
    init_address_space(addrSpaceId);
}

void FlowKey::init(
    PktType type, IpProtocol ip_proto,
    const sfip_t *srcIP, const sfip_t *dstIP,
    uint32_t id, uint16_t vlanId,
    uint32_t mplsId, uint16_t addrSpaceId)
{
    // to avoid confusing 2 different datagrams or confusing a datagram
    // with a session, we don't order the addresses and we set version
    uint16_t srcPort = id & 0xFFFF;
    uint16_t dstPort = id >> 16;

    if (srcIP->is_ip4())
    {
        version = 4;
        init4(ip_proto, srcIP, srcPort, dstIP, dstPort, mplsId, false);
    }
    else
    {
        version = 6;
        init6(ip_proto, srcIP, srcPort, dstIP, dstPort, mplsId, false);
    }
    pkt_type = type;

    init_vlan(vlanId);
    init_address_space(addrSpaceId);
}

//-------------------------------------------------------------------------
// hash foo
//-------------------------------------------------------------------------

uint32_t FlowKey::hash(SFHASHFCN*, unsigned char* d, int)
{
    uint32_t a,b,c;

    a = *(uint32_t*)d;         /* IPv6 lo[0] */
    b = *(uint32_t*)(d+4);     /* IPv6 lo[1] */
    c = *(uint32_t*)(d+8);     /* IPv6 lo[2] */

    mix(a,b,c);

    a += *(uint32_t*)(d+12);   /* IPv6 lo[3] */
    b += *(uint32_t*)(d+16);   /* IPv6 hi[0] */
    c += *(uint32_t*)(d+20);   /* IPv6 hi[1] */

    mix(a,b,c);

    a += *(uint32_t*)(d+24);   /* IPv6 hi[2] */
    b += *(uint32_t*)(d+28);   /* IPv6 hi[3] */
    c += *(uint32_t*)(d+32);   /* port lo & port hi */

    mix(a,b,c);

    a += *(uint32_t*)(d+36);    /* vlan tag, packet type, & version */
    b += *(uint32_t*)(d+40);    /* mpls label */
    c += *(uint32_t*)(d+44);    /* address space id and 16bits of zero'd pad */

    finalize(a,b,c);

    return c;
}

int FlowKey::compare(const void* s1, const void* s2, size_t)
{
#ifndef SPARCV9 /* ie, everything else, use 64bit comparisons */
    uint64_t* a,* b;

    a = (uint64_t*)s1;
    b = (uint64_t*)s2;
    if (*a - *b)
        return 1;               /* Compares IPv4 lo/hi
                                   Compares IPv6 low[0,1] */

    a++;
    b++;
    if (*a - *b)
        return 1;               /* Compares port lo/hi, vlan, protocol, version
                                   Compares IPv6 low[2,3] */

    a++;
    b++;
    if (*a - *b)
        return 1;               /* Compares IPv6 hi[0,1] */

    a++;
    b++;
    if (*a - *b)
        return 1;               /* Compares IPv6 hi[2,3] */

    a++;
    b++;
    if (*a - *b)
        return 1;               /* Compares port lo/hi, vlan, protocol, version */

    a++;
    b++;
    if (*a - *b)
        return 1;               /* Compares MPLS label, AddressSpace ID and 16bit pad */

#else /* SPARCV9 */
    uint32_t* a,* b;

    a = (uint32_t*)s1;
    b = (uint32_t*)s2;
    if ((*a - *b) || (*(a+1) - *(b+1)))
        return 1;                                       /* Compares IPv4 lo/hi */
    /* Compares IPv6 low[0,1] */

    a+=2;
    b+=2;
    if ((*a - *b) || (*(a+1) - *(b+1)))
        return 1;                                       /* Compares port lo/hi, vlan, protocol,
                                                          version */
    /* Compares IPv6 low[2,3] */

    a+=2;
    b+=2;
    if ((*a - *b) || (*(a+1) - *(b+1)))
        return 1;                                       /* Compares IPv6 hi[0,1] */

    a+=2;
    b+=2;
    if ((*a - *b) || (*(a+1) - *(b+1)))
        return 1;                                       /* Compares IPv6 hi[2,3] */

    a+=2;
    b+=2;
    if ((*a - *b) || (*(a+1) - *(b+1)))
        return 1;                                       /* Compares port lo/hi, vlan, protocol,
                                                          version */

    a+=2;
    b+=2;
    {
        uint32_t* x, * y;
        x = (uint32_t*)a;
        y = (uint32_t*)b;
        // x++;
        // y++;
        if (*x - *y)
            return 1;           /* Compares mpls label */
    }
    a++;
    b++;
    {
        uint16_t* x, * y;
        x = (uint16_t*)a;
        y = (uint16_t*)b;
        // x++;
        // y++;
        if (*x - *y)
            return 1;           /* Compares addressSpaceID, no pad */
    }
#endif /* SPARCV9 */

    return 0;
}

