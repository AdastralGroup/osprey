#pragma once

#include <map>
#include <string>

enum class EventType
{
    OnUpdate = 0,
    OnError = 1,
    Max = 2
};

class Event
{
  public:
    Event(EventType event_type);
    EventType get_type();

  private:
    EventType type;
};
