#pragma once

#include <events/event.hpp>
#include <functional>
#include <map>
#include <vector>

class EventSystem {
  using EventLambda = std::function<void(Event&)>;

 public:
  EventSystem();
  void RegisterListener(EventType eventType, const EventLambda& fun);
  void TriggerEvent(Event& data);

 private:
  std::map<EventType, std::vector<EventLambda>> m_subscribers;
};
