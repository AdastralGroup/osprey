/*
 *   Copyright (c) 2023 Copyright The Adastral Group and Contributors

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
#include <coldfield/AdastralDefines.h>

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/signal.hpp>
//=============================

using namespace godot;
namespace adastral {
/// @brief Helper class for subscribing to events fired by GDscript.
/// NOTE: events = signals in Godot's eyes.

class AD_COLDFIELD_DLL ADProjectEvent : public Node {
  GDCLASS(ADProjectEvent, Node);

 private:
  Object* target;     // The target object that emits the signal
  String event_name;  // The name of the event
  Signal target_signal;

 public:
  inline void connect_to_event(Object* target, const String& event_name) {
    this->target = target;
    this->event_name = event_name;

    if (target) {
      StringName signal_name(event_name);
      target->connect(signal_name, (const Callable&)target);
    }
  }

  inline void disconnect_from_event() {
    if (target && !event_name.is_empty()) {
      StringName signal_name(event_name);
      target->disconnect(signal_name, (const Callable&)target);
    }

    target = nullptr;
    event_name = "";
  }

  void _on_event_triggered() {
    // Handle the event here
    // target_signal.emit("event_triggered");
  }
  static void _bind_methods() {}
  static void _register_methods() {
    // ClassDB::bind_method("_on_event_triggered", &ADProjectEvent::_on_event_triggered);

    // Expose methods to GDScript
    ClassDB::bind_method("connect_to_event", &ADProjectEvent::connect_to_event, "connect_to_event");
    ClassDB::bind_method("disconnect_from_event",&ADProjectEvent::disconnect_from_event,"connect_to_event");
  }
};
}  // namespace adastral