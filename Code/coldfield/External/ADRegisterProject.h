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
#include <coldfield/AdastralDefines.h>

#include <godot_cpp/classes/jni_singleton.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <map>
//=============================
using namespace godot;
namespace adastral {
/// @brief Helper class to assist with registering external libraries to godot.
class ADProjectRegister : public godot::Node {
  GDCLASS(ADProjectRegister, godot::Node)
 public:
  ADProjectRegister();
  virtual ~ADProjectRegister();
  static void _bind_methods() {
    // godot::ClassDB::bind_method_godot("_register_library", (godot::MethodBind*)ADProjectRegister::RegisterClass);
    // godot::ClassDB::bind_method_godot("_unregister_library", ADProjectRegister::RegisterClass);
    {
      auto ___call = [](GDExtensionObjectPtr p_instance, const GDExtensionConstTypePtr* p_args,
                        GDExtensionTypePtr p_ret) -> void {};
      godot::ClassDB::bind_virtual_method(ADProjectRegister::get_class_static(), "RegisterClass", ___call);
      godot::ClassDB::bind_virtual_method(ADProjectRegister::get_class_static(), "BindGodotMethod", ___call);
    }
  }
  /// @brief registers a class within godot.
  template <class a>
  void RegisterClass(godot::String _classname, a regclass);

  /// @brief Binds a godot method. NOTE: This should be used for static methods/classes. it will not be assigned to a
  /// class (In-terms of godot).
  template <class s>
  void BindGodotMethod(s& method, godot::String& _methodname);

  /// @brief Destroys the class db, which in turn, de-init's all registered classes.
  void KillClasses(GDExtensionInitializationLevel& a);

 protected:
 private:
  std::map<godot::String, void*> _registeredprojects;
  godot::JNISingleton _classsingleton;
};

template <class s>
void adastral::ADProjectRegister::BindGodotMethod(s& method, godot::String& _methodname) {
  godot::ClassDB::bind_method_godot(_methodname, method);
  _registeredprojects.try_emplace(_methodname, method);
}

template <class a>
void adastral::ADProjectRegister::RegisterClass(godot::String _classname, a& regclass) {
  godot::ClassDB::register_class<a>();
  _registeredprojects.try_emplace(_classname, regclass);
}
}  // namespace adastral