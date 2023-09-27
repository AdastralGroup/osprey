#include <events/progress.hpp>

ProgressUpdate::ProgressUpdate(float bps, float progress) : Event(EventType::PROGRESS_UPDATE)
{
	m_fBps = bps;
	m_fProgress = progress;
}

float ProgressUpdate::GetBps() {
	return m_fBps;
}

float ProgressUpdate::GetProgress() {
	return m_fProgress;
}
