/*
 *   Copyright (c) 2023 Adastral Group and Contributors

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
 /// NOTE: Visual Studio will give out false errors when compiling for the first time. this is normal with gd. the manager SHOULD compile normally after building godot.
#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>

void register_gameplay_types(godot::ModuleInitializationLevel p_level) {
	if (p_level != godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	// REGISTER CLASSES HERE LATER
}

void unregister_gameplay_types(godot::ModuleInitializationLevel p_level) {
	// DO NOTHING
}

extern "C" {
	GDExtensionBool GDE_EXPORT gameplay_library_init(const GDExtensionInterface* p_interface, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization) {
		godot::GDExtensionBinding::InitObject init_object(p_interface, p_library, r_initialization);

		init_object.register_initializer(register_gameplay_types);
		init_object.register_terminator(unregister_gameplay_types);
		init_object.set_minimum_library_initialization_level(godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_object.init();
	}
}