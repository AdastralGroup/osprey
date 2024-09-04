#pragma once

#include <map>
#include <string>

enum class EventType
{
    kOnUpdate = 0,
    kOnError = 1,
    kMax = 2
};

class Event
{
  public:
    Event(EventType event_type);
    EventType get_type();

  private:
    EventType M_type;
};
