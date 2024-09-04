#include <event.hpp>

Event::Event(EventType type)
{
    M_type = type;
}

EventType Event::get_type()
{
    return M_type;
}
