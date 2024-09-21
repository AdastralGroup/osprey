#include <error.hpp>
#include <utility>

ErrorMessage::ErrorMessage(std::string message, ErrorLevel error_level) : Event(EventType::OnError)
{
    this->message = std::move(message);
    this->error_level = error_level;
}

const std::string &ErrorMessage::get_message()
{
    return this->message;
}

const ErrorLevel &ErrorMessage::get_error_level()
{
    return this->error_level;
}