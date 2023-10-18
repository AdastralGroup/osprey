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

 // Configure the DLL Import/Export Define

#  ifdef BUILDSYSTEM_BUILDING_COLDFIELD_LIB
#    ifdef _WIN32
#      define AD_COLDFIELD_DLL __declspec(dllexport)
#    else
#      define AD_COLDFIELD_DLL [[gnu::visibility("default")]]
#    endif
#  else
#      ifdef _WIN32
#           define AD_COLDFIELD_DLL __declspec(dllimport)
#      else
#         define define AD_COLDFIELD_DLL [[gnu::visibility("default")]]
#       endif
#  endif

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>