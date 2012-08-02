#include "common.hpp"

#include "eventtype.hpp"

const std::string EventType::wildcardType = "*";

EventType::EventType(const std::string &type)
{
	_idString = type;
	_id       = hashName(type);
}

EventType::~EventType()
{
}

int EventType::getId() const
{
	return _id;
}

const std::string &EventType::getString() const
{
	return _idString;
}

bool EventType::operator<(const EventType &rhs) const
{
	return (getId() < rhs.getId());
}

bool EventType::operator==(const EventType &rhs) const
{
	return (getId() == rhs.getId());
}

// This string hashing function was obtained from:
// http://www.cs.yorku.ca/~oz/hash.html
// Looks like it's called djb2
int EventType::hashName(const std::string &type)
{
	// Return 0 if we got passed the wildcard type, meaning that 
	// this should match any event.
	if (type == wildcardType)
		return 0;

	const char *typeStr = type.c_str();
	assert(typeStr != 0);

	int hash = 5381;
	int c;

	while (c = *typeStr++)
		hash = ((hash << 5) + hash) + c; // hash * 33 + c

	return hash;
}
