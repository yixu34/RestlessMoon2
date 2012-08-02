#ifndef PACKET_HPP
#define PACKET_HPP

//
// Raw data buffer that serves two useful purposes:
// 1) For initializing events and processes via a script file.  This gives
//    us runtime flexibility, at the expense of static typing.
// 2) If we ever make this thing multiplayer, then this will encapsulate data
//    sent over the network.
//

#include <string>
#include <boost/shared_ptr.hpp>

class Packet;
typedef boost::shared_ptr<Packet> PacketPtr;

class Packet
{
public:
	explicit Packet();
	explicit Packet(int type);
	Packet(char *dataWithHeader, int sizeWithoutHeader);
	~Packet();
	
	void putChar(char value);
	void putShort(short value);
	void putInt(int value);
	void putFloat(float value);
	void putString(const std::string &value);
	
	char  getChar();
	short getShort();
	int   getInt();
	float getFloat();
	std::string getString();
	
	const char *getData();
	int getSize() const;

	Packet *clone();
	
private:
	// Pass by value?  I think not, sir!
	Packet(const Packet &rhs);  

	void init();
	void expand(int numBytes);

	unsigned char *_data;
	int _pos;
	int _size;
	int _alloc;
};

#endif
