#include "packet.hpp"
#include <cstdlib>
#include <cstring>

Packet::Packet()
{
	init();
}

Packet::Packet(int type)
{
	init();
	putInt(type);
}

Packet::Packet(
	char *dataWithHeader, 
	int sizeWithoutHeader)
{
	_pos  = 4;
	_size = _alloc = sizeWithoutHeader + sizeof(int);

	_data = static_cast<unsigned char *>(malloc(_alloc));
	memcpy(_data, dataWithHeader, _size);
	memcpy(_data, &_size, sizeof(int));
}

void Packet::init()
{
	_pos   = 4;
	_size  = 4;
	_alloc = 32 + sizeof(int);   

	_data  = static_cast<unsigned char*>(malloc(_alloc));
	memset(_data, 0, _alloc);
	memcpy(_data, &_size, sizeof(int));
}

Packet::~Packet()
{
	free(_data);
}

//////////////////////////////////////////////////////////////////////////

void Packet::putChar(char value)
{
	expand(1);
	_data[_size++] = value;
}

void Packet::putShort(short value)
{
	expand(2);
	_data[_size++] = (value      & 0xFF);
	_data[_size++] = (value >> 8 & 0xFF);
}

void Packet::putInt(int value)
{
	expand(4);
	_data[_size++] = (value       & 0xFF);
	_data[_size++] = (value >> 8  & 0xFF);
	_data[_size++] = (value >> 16 & 0xFF);
	_data[_size++] = (value >> 24 & 0xFF);
}

void Packet::putFloat(float value)
{
	expand(sizeof(float));
	memcpy(_data + _size, &value, sizeof(float));
	_size += sizeof(float);
}

void Packet::putString(const std::string &value)
{
	const char *toPut = value.c_str();

	// Make room for the null terminator too
	expand(static_cast<int>(strlen(toPut) + 1));

	while (*toPut != '\0')
		_data[_size++] = *(toPut++);

	_data[_size++] = '\0';
}

//////////////////////////////////////////////////////////////////////////

char Packet::getChar()
{
	return _data[_pos++];
}

short Packet::getShort()
{
	short ret = 
		(static_cast<unsigned>(_data[_pos    ])) |
		(static_cast<unsigned>(_data[_pos + 1])) << 8;

	_pos += 2;
	return ret;
}

int Packet::getInt()
{
	int ret = 
		(static_cast<unsigned>(_data[_pos    ]))       |
		(static_cast<unsigned>(_data[_pos + 1])) << 8  |
		(static_cast<unsigned>(_data[_pos + 2])) << 16 |
		(static_cast<unsigned>(_data[_pos + 3])) << 24;

	_pos += 4;
	return ret;
}

float Packet::getFloat()
{
	float ret;
	memcpy(&ret, _data + _pos, sizeof(float));
	_pos += sizeof(float);
	return ret;
}

std::string Packet::getString()
{
	std::string ret("");
	while (_data[_pos] != '\0')
		ret += _data[_pos++];

	++_pos;  // Skip over the null terminator

	return ret;
}

//////////////////////////////////////////////////////////////////////////

int Packet::getSize() const
{
	return _size;
}

const char *Packet::getData()
{
	// About to pass this _data over the network, so encode the size also
	memcpy(_data, &_size, sizeof(int));
	return reinterpret_cast<char *>(_data);
}

void Packet::expand(int numBytes)
{
	while (_size + numBytes >= _alloc)
		_alloc *= 2;

	_data = static_cast<unsigned char *>(realloc(_data, _alloc));
}

Packet *Packet::clone()
{
	return new Packet(
		reinterpret_cast<char *>(this->_data), 
		_size - sizeof(int));
}