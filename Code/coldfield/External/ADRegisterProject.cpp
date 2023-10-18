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
#include <coldfield/External/ADRegisterProject.h>

adastral::ADProjectRegister::ADProjectRegister()
{
	_classsingleton.initialize_class();
}

adastral::ADProjectRegister::~ADProjectRegister()
{
	_registeredprojects.clear();
}

void adastral::ADProjectRegister::KillClasses(GDExtensionInitializationLevel& a)
{
	
}