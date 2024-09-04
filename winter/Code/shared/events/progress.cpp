#include <progress.hpp>

ProgressUpdateMessage::ProgressUpdateMessage(long double bps, long double progress) : Event(EventType::OnUpdate)
{
    bps = bps;
    progress = progress;
}

long double ProgressUpdateMessage::get_bps()
{
    return bps;
}

long double ProgressUpdateMessage::get_progress()
{
    return progress;
}
