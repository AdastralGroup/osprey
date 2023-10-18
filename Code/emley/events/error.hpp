#pragma once

#include <events/event.hpp>

class ErrorMessage : public Event {
public:
	ErrorMessage(std::string message);
	const std::string& GetMessage();
private:
	std::string m_szMessage;
};

