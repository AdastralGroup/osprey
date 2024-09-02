#pragma once

#include <events/event.hpp>
#include <functional>
#include <map>
#include <vector>

class EventSystem {
  using EventLambda = std::function<void(Event&)>;

 public:
  EventSystem();
  void register_listener(EventType event_type, const EventLambda& fun);
  void trigger_event(Event& data);

 private:
  std::map<EventType, std::vector<EventLambda>> subscribers;
};
