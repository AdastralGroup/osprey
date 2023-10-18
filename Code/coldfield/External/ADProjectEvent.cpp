#include <coldfield/External/ADProjectEvent.h>

adastral::ADProjectEvent::ADProjectEvent()
{

}

adastral::ADProjectEvent::~ADProjectEvent()
{

}

void adastral::ADProjectEvent::FireEvent(godot::Signal* _connectobject, godot::String _eventname, godot::String message)
{
	_connectobject->emit(_eventname, message);
}


