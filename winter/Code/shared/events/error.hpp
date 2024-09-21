#pragma once

#include "event.hpp"

enum ErrorLevel
{
    INFO = 0,
    WARNING = 1,
    SERIOUS = 2,
    OOPS = 3,
    PANIC = 4
};


class ErrorMessage : public Event
{
  public:
    ErrorMessage(std::string message, ErrorLevel error_level);
    const std::string &get_message();
    const ErrorLevel &get_error_level();

  private:
    ErrorLevel error_level;
    std::string message;
};
