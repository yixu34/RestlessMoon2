#ifndef EVENTTYPE_HPP
#define EVENTTYPE_HPP

//
// Encapsulates a string hash for event types.
//

#include <string>

class EventType
{
public:
	explicit EventType(const std::string &type);
	~EventType();

	int getId() const;
	const std::string &getString() const;

	bool operator<(const EventType &rhs) const;
	bool operator==(const EventType &rhs) const;

	static int hashName(const std::string &type);
	static const std::string wildcardType;

private:
	int _id;
	std::string _idString;
};

#endif
