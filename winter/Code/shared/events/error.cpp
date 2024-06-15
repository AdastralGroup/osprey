#include <error.hpp>

ErrorMessage::ErrorMessage(std::string message) : Event(EventType::kOnError) { m_szMessage = message; }

const std::string& ErrorMessage::get_message() { return m_szMessage; }
