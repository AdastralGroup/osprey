#include <error.hpp>

ErrorMessage::ErrorMessage(std::string message) : Event(EventType::kOnError) { m_szMessage = message; }

const std::string& ErrorMessage::GetMessage() { return m_szMessage; }
