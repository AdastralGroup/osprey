/*
 *   Copyright (c) 2023 Copyright Adastral Group and Contributors

 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.

 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.

 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
 //====== Needed Headers ===========
#include <AdastralManager/AdastralDefines.h>
#include <godot_cpp/variant/callable.hpp>
//=============================

namespace adastral
{
	/// @brief Helper class for subscribing to events fired by GDscript.
	/// NOTE: events = signals in Godot's eyes.
	class AD_ADASTRALMANAGER_DLL ADProjectEvent 
	{
	public:
		ADProjectEvent();
		virtual ~ADProjectEvent();
		
		/// @brief Subscribes to events fired by GDScript. 
		/// On Godot's end, this will connect through a signal to the desired object.
		/// NOTE: The Function Name MUST be already Registered in The ClassDB To work!
		void TrySubscribing(godot::String _eventname, void* handle, godot::String _funcname, godot::Object* _connectingobject);

		/// @brief Fires events(signals) back to GDscript.
		void FireEvent();
	protected:
		/// @brief Binds all requested events to Godot. should be called when methods are bound.
		/// EX: Should be called within "GDExample::_bind_methods()"
		void BindSubscribedEvents();
	private:
	};

	void ADProjectEvent::TrySubscribing(godot::String _eventname, void* handle, godot::String _funcname, godot::Object* _connectingobject)
	{
		/// NOTE: Providing the _connectingobject wont work well here, we could template the class and reference it that way? 
	}

}