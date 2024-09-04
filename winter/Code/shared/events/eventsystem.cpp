#include <eventsystem.hpp>

EventSystem::EventSystem()
{
    for (int i = 0; i < (int)EventType::kMax; i++)
    {
        subscribers[(EventType)i] = std::vector<EventLambda>();
    }
}

void EventSystem::register_listener(EventType event_type, const EventLambda &fun)
{
    subscribers.at(event_type).push_back(fun);
}

void EventSystem::trigger_event(Event &data)
{
    // if (!subscribers.contains(data.get_type()))
    //	return;

    std::vector<EventLambda> &events = subscribers.at(data.get_type());
    for (EventLambda &ev : events)
    {
        ev(data);
    }
}
