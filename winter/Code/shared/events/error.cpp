#include <error.hpp>

ErrorMessage::ErrorMessage(std::string message) : Event(EventType::OnError)
{
    this->message = message;
}

const std::string &ErrorMessage::get_message()
{
    return message;
}
