#include <progress.hpp>

ProgressUpdateMessage::ProgressUpdateMessage(float bps, float progress) : Event(EventType::kOnUpdate) {
  m_fBps = bps;
  m_fProgress = progress;
}

float ProgressUpdateMessage::GetBps() { return m_fBps; }

float ProgressUpdateMessage::GetProgress() { return m_fProgress; }
