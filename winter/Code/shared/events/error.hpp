#pragma once

#include "event.hpp"

class ErrorMessage : public Event {
 public:
  ErrorMessage(std::string message);
  const std::string& get_message();

 private:
  std::string m_szMessage;
};
