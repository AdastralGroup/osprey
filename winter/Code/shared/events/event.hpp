#pragma once

#include <map>
#include <string>

enum class EventType { kOnUpdate = 0, kOnError = 1, kMax = 2 };

class Event {
 public:
  Event(EventType eventType);
  EventType GetType();

 private:
  EventType m_type;
};
