#include <progress.hpp>

ProgressUpdateMessage::ProgressUpdateMessage(long double bps, long double progress) : Event(EventType::OnUpdate)
{
    this->bps = bps;
    this->progress = progress;
}

long double ProgressUpdateMessage::get_bps()
{
    return bps;
}

long double ProgressUpdateMessage::get_progress()
{
    return progress;
}
