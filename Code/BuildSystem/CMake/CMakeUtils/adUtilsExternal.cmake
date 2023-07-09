# #####################################
# ## ad_include_wdExport()
# #####################################

macro(ad_include_wdExport)
	# Create a modified version of the wdExport.cmake file,
	# where the absolute paths to the original locations are replaced
	# with the absolute paths to this installation
	set(EXP_FILE "${AD_OUTPUT_DIRECTORY_DLL}/AdastralExport.cmake")
	set(IMP_FILE "${CMAKE_BINARY_DIR}/AdastralExport.cmake")
	set(EXPINFO_FILE "${AD_OUTPUT_DIRECTORY_DLL}/AdastralExportInfo.cmake")

	# read the file that contains the original paths
	include(${EXPINFO_FILE})

	# read the wdExport file into a string
	file(READ ${EXP_FILE} IMP_CONTENT)

	# replace the original paths with our paths
	string(REPLACE ${EXPINP_OUTPUT_DIRECTORY_DLL} ${AD_OUTPUT_DIRECTORY_DLL} IMP_CONTENT "${IMP_CONTENT}")
	string(REPLACE ${EXPINP_OUTPUT_DIRECTORY_LIB} ${AD_OUTPUT_DIRECTORY_LIB} IMP_CONTENT "${IMP_CONTENT}")
	string(REPLACE ${EXPINP_SOURCE_DIR} ${AD_SDK_DIR} IMP_CONTENT "${IMP_CONTENT}")

	# write the modified wdExport file to disk
	file(WRITE ${IMP_FILE} "${IMP_CONTENT}")

	# include the modified file, so that the CMake targets become known
	include(${IMP_FILE})
endmacro()

# #####################################
# ## ad_configure_external_project()
# #####################################
macro(ad_configure_external_project)

	if (AD_SDK_DIR STREQUAL "")
		file(RELATIVE_PATH AD_SUBMODULE_PREFIX_PATH ${CMAKE_SOURCE_DIR} ${AD_SDK_DIR})
	else()
		set(AD_SUBMODULE_PREFIX_PATH "")
	endif()

	set_property(GLOBAL PROPERTY AD_SUBMODULE_PREFIX_PATH ${AD_SUBMODULE_PREFIX_PATH})

	if(AD_SUBMODULE_PREFIX_PATH STREQUAL "")
		set(AD_SUBMODULE_MODE FALSE)
	else()
		set(AD_SUBMODULE_MODE TRUE)
	endif()

	set_property(GLOBAL PROPERTY AD_SUBMODULE_MODE ${AD_SUBMODULE_MODE})

	ad_build_filter_init()

	ad_set_build_types()
endmacro()