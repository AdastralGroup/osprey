# #####################################
# ## ad_vcpkg_show_info()
# #####################################

function(ad_vcpkg_show_info)
	message("This project can use vcpkg to find its 3rd party dependencies.")
	message("You can install vcpkg either manually or using InstallVcpkg.bat.")
	message("Make sure to set the environment variable 'VCPKG_ROOT' to point to your installation of vcpkg.")
	message("")
endfunction()

# #####################################
# ## ad_vcpkg_init()
# #####################################
function(ad_vcpkg_init)
	message(STATUS "EnvVar %VCPKG_ROOT% is set to '$ENV{VCPKG_ROOT}'")

	if(CMAKE_TOOLCHAIN_FILE)
		message(STATUS "CMAKE_TOOLCHAIN_FILE is already set to '${CMAKE_TOOLCHAIN_FILE}' - not going to modify it.")
		get_filename_component(AD_VCPKG_ROOT "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
		get_filename_component(AD_VCPKG_ROOT "${AD_VCPKG_ROOT}" DIRECTORY)
		get_filename_component(AD_VCPKG_ROOT "${AD_VCPKG_ROOT}" DIRECTORY)

	else()
		if(DEFINED ENV{VCPKG_ROOT})
			set(AD_VCPKG_ROOT "$ENV{VCPKG_ROOT}")
			message(STATUS "EnvVar VCPKG_ROOT is specified, using that.")
		else()
			set(AD_VCPKG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg")
			message(STATUS "EnvVar VCPKG_ROOT is not specified, using '${CMAKE_CURRENT_SOURCE_DIR}/vcpkg'.")
		endif()

		if(NOT EXISTS "${AD_VCPKG_ROOT}/vcpkg.exe" OR NOT EXISTS "${AD_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
			message("vcpkg is not installed. Either install it manually and set the environment variable VCPKG_ROOT to its directory, or run InstallVcpkg.bat")
			return()
		endif()

		set(CMAKE_TOOLCHAIN_FILE ${AD_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "" FORCE)
		message(STATUS "Forcing CMAKE_TOOLCHAIN_FILE to point to '${CMAKE_TOOLCHAIN_FILE}'")
	endif()

	message(STATUS "AD_VCPKG_ROOT is '${AD_VCPKG_ROOT}'")
	set_property(GLOBAL PROPERTY "AD_VCPKG_ROOT" ${AD_VCPKG_ROOT})
endfunction()

# #####################################
# ## ad_vcpkg_install(<packages>)
# #####################################
function(ad_vcpkg_install PACKAGES)
	ad_cmake_init()

	get_property(AD_VCPKG_ROOT GLOBAL PROPERTY "AD_VCPKG_ROOT")

	if(AD_CMAKE_PLATFORM_WINDOWS)
		if(AD_CMAKE_ARCHITECTURE_64BIT)
			set(VCPKG_TARGET_TRIPLET "x64-windows")
		else()
			set(VCPKG_TARGET_TRIPLET "x86-windows")
		endif()
	else()
		message(FATAL_ERROR "vcpkg target triplet is not configured for this platform")
	endif()

	foreach(PACKAGE ${PACKAGES})
		message("Vcpgk: Installing '${PACKAGE}', this may take a while.")
		execute_process(COMMAND "${AD_VCPKG_ROOT}/vcpkg.exe" install "${PACKAGE}:${VCPKG_TARGET_TRIPLET}" WORKING_DIRECTORY "${AD_VCPKG_ROOT}")
	endforeach()
endfunction()
