#pragma once
#include "event.hpp"

class ProgressUpdateMessage : public Event
{
  public:
    ProgressUpdateMessage(long double bps, long double progress);
    long double get_bps();
    long double get_progress();

  private:
    long double bps;
    long double progress;
};
