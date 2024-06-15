#include <eventsystem.hpp>

EventSystem::EventSystem() {
  for (int i = 0; i < (int)EventType::kMax; i++) {
    m_subscribers[(EventType)i] = std::vector<EventLambda>();
  }
}

void EventSystem::RegisterListener(EventType eventType, const EventLambda& fun) {
  m_subscribers.at(eventType).push_back(fun);
}

void EventSystem::TriggerEvent(Event& data) {
  // if (!m_subscribers.contains(data.GetType()))
  //	return;

  std::vector<EventLambda>& events = m_subscribers.at(data.GetType());
  for (EventLambda& ev : events) {
    ev(data);
  }
}
