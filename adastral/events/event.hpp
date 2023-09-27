#pragma once

#include <string>
#include <map>

enum class EventType {
	PROGRESS_UPDATE = 0,
	EVENTTYPE_MAX = 1
};

class Event {
public:
	Event(EventType eventType);
	EventType GetType();
private:
	EventType m_type;
};
