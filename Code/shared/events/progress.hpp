#pragma once
#include "event.hpp"

class ProgressUpdateMessage : public Event {
 public:
  ProgressUpdateMessage(float bps, float progress);
  float GetBps();
  float GetProgress();

 private:
  float m_fBps;
  float m_fProgress;
};
