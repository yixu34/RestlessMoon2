#ifndef PACKETCONSTRUCTABLE_HPP
#define PACKETCONSTRUCTABLE_HPP

//
// Abstract base class for an object that can be made from a packet at runtime.
//

#include <boost/shared_ptr.hpp>

class Packet;
typedef boost::shared_ptr<Packet> PacketPtr;

class PacketConstructable
{
public:
	PacketConstructable() {}
	virtual ~PacketConstructable() {}

	virtual bool readFromPacket(PacketPtr packet) = 0;
};

#endif