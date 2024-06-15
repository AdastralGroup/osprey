#include <progress.hpp>

ProgressUpdateMessage::ProgressUpdateMessage(long double bps, long double progress) : Event(EventType::kOnUpdate) {
  m_fBps = bps;
  m_fProgress = progress;
}

long double ProgressUpdateMessage::GetBps() { return m_fBps; }

long double ProgressUpdateMessage::GetProgress() { return m_fProgress; }
