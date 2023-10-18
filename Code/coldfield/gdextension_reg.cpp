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


#include <coldfield/AdastralDefines.h>


#include <coldfield/GDRegister/register_adastral_types.h>
#include <coldfield/External/ADProjectEvent.h>
#include <coldfield/External/ADRegisterProject.h>

void register_adastral_types(godot::ModuleInitializationLevel p_level) {
	if (p_level != godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	godot::ClassDB::register_abstract_class<adastral::ADProjectEvent>();
	godot::ClassDB::register_abstract_class<adastral::ADProjectRegister>();
	// REGISTER CLASSES HERE LATER
}

void unregister_adastral_types(godot::ModuleInitializationLevel p_level) {
	// DO NOTHING
}

extern "C" {
	GDExtensionBool GDE_EXPORT adastral_library_init(const GDExtensionInterface* p_interface, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization) {
		godot::GDExtensionBinding::InitObject init_object(p_interface, p_library, r_initialization);

		init_object.register_initializer(register_adastral_types);
		init_object.register_terminator(unregister_adastral_types);
		init_object.set_minimum_library_initialization_level(godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_object.init();
	}
}