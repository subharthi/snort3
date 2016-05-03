
//struct random_packet {
//    random_packet(int seed) : gen(seed) { }
//
//    Packet operator()() {
//        boost::random::uniform_int_distribution<> port_range(0, 65535);
//        boost::random::uniform_int_distribution<> ip_range(0, 10); // just 10 ip addresses possible
//        boost::random::uniform_int_distribution<> protocol_range(0, 255);
//        return Packet(ip_range(gen), ip_range(gen), port_range(gen), port_range(gen), protocol_range(gen));
//
//    }
//
//private:
//    boost::mt19937 gen;
//};
