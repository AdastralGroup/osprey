#include <progress.hpp>

ProgressUpdateMessage::ProgressUpdateMessage(long double bps, long double progress) : Event(EventType::kOnUpdate)
{
    M_bps = bps;
    M_progress = progress;
}

long double ProgressUpdateMessage::get_bps()
{
    return M_bps;
}

long double ProgressUpdateMessage::get_progress()
{
    return M_progress;
}
