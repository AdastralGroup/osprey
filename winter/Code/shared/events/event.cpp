#include <event.hpp>

Event::Event(EventType type)
{
    this->type = type;
}

EventType Event::get_type()
{
    return type;
}
