
#include <events/event.hpp>

class ProgressUpdate : public Event {
public:
	ProgressUpdate(float bps, float progress);
	float GetBps();
	float GetProgress();
private:
	float m_fBps;
	float m_fProgress;
};
