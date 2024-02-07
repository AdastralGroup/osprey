#pragma once
#include "event.hpp"

class ProgressUpdateMessage : public Event {
 public:
  ProgressUpdateMessage(long double bps, long double progress);
  long double GetBps();
  long double GetProgress();

 private:
  long double m_fBps;
  long double m_fProgress;
};
