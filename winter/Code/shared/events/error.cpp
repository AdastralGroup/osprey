#include <error.hpp>

ErrorMessage::ErrorMessage(std::string message) : Event(EventType::kOnError) { M_message = message; }

const std::string& ErrorMessage::get_message() { return M_message; }
