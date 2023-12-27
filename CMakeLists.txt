cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
project("winter" VERSION 1.0 LANGUAGES CXX)
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds are not supported. Please choose a different binary directory.")
	return()
endif()

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()
include(FetchContent)
string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC" )
	set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC" )
endif()

#include("${CMAKE_CURRENT_SOURCE_DIR}/AdastralCMakeConfig.cmake")
#get_property(AD_CMAKE_RELPATH GLOBAL PROPERTY AD_CMAKE_RELPATH)
#get_property(AD_CMAKE_RELPATH_CODE GLOBAL PROPERTY AD_CMAKE_RELPATH_CODE)
#
# Required to find cmake modules like 'FindDirectX.cmake'
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/${AD_CMAKE_RELPATH}")

#include("Code/BuildSystem/CMake/adUtils.cmake")
#
#file(RELATIVE_PATH AD_SUBMODULE_PREFIX_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
#set_property(GLOBAL PROPERTY AD_SUBMODULE_PREFIX_PATH ${AD_SUBMODULE_PREFIX_PATH})
#
#if(AD_SUBMODULE_PREFIX_PATH STREQUAL "")
#	set(AD_SUBMODULE_MODE FALSE)
#else()
#	set(AD_SUBMODULE_MODE TRUE)
#endif()
#
#set_property(GLOBAL PROPERTY AD_SUBMODULE_MODE ${AD_SUBMODULE_MODE})
#
#ad_build_filter_init()
#
#if(NOT AD_SUBMODULE_MODE)
#	ad_detect_languages()

#endif()
#
#ad_set_build_types()
#ad_check_build_type()
#ad_write_configuration_txt()
#
#if(AD_CUSTOM_TOOLCHAIN_FILE)
#	message(STATUS "Including AD_CUSTOM_TOOLCHAIN_FILE: '${AD_CUSTOM_TOOLCHAIN_FILE}'")
#	include("${AD_CUSTOM_TOOLCHAIN_FILE}")
#endif()
#
#ad_pull_all_vars()
#
#include("adPlatformVars")
#
#set(AD_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
#
#ad_init_projects()
#add_link_options(" -fno-use-linker-plugin")
#add_compile_options("/p:CLToolPath=C:\\ProgramData\\chocolatey\\lib\\sccache\\tools\\sccache-v0.7.4-x86_64-pc-windows-msvc\\sccache.exe")
add_subdirectory(Code)

#ad_finalize_projects()
#
#
#get_property(EXPORT_PROJECTS GLOBAL PROPERTY "EXPORT_PROJECTS")
#export(TARGETS ${EXPORT_PROJECTS} FILE "${AD_OUTPUT_DIRECTORY_DLL}/AdastralExport.cmake")
#
#set(EXPORT_INFO "
#set(EXPINP_OUTPUT_DIRECTORY_DLL ${AD_OUTPUT_DIRECTORY_DLL})
#set(EXPINP_OUTPUT_DIRECTORY_LIB ${AD_OUTPUT_DIRECTORY_LIB})
#set(EXPINP_BINARY_DIR ${CMAKE_BINARY_DIR})
#set(EXPINP_SOURCE_DIR ${CMAKE_SOURCE_DIR})
#")
#
#file(WRITE "${AD_OUTPUT_DIRECTORY_DLL}/AdastralExportInfo.cmake" ${EXPORT_INFO})
