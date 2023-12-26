include(CheckIncludeFileCXX)

file(GLOB UTILS_FILES "${CMAKE_CURRENT_LIST_DIR}/CMakeUtils/*.cmake")

# automatically include all files in the CMakeUtils subfolder
foreach (UTILS_FILE ${UTILS_FILES})
    include("${UTILS_FILE}")
endforeach ()

# #####################################
# ## ad_pull_config_vars()
# #####################################
macro(ad_pull_config_vars)
    get_property(AD_BUILDTYPENAME_DEBUG GLOBAL PROPERTY AD_BUILDTYPENAME_DEBUG)
    get_property(AD_BUILDTYPENAME_DEV GLOBAL PROPERTY AD_BUILDTYPENAME_DEV)
    get_property(AD_BUILDTYPENAME_RELEASE GLOBAL PROPERTY AD_BUILDTYPENAME_RELEASE)

    get_property(AD_BUILDTYPENAME_DEBUG_UPPER GLOBAL PROPERTY AD_BUILDTYPENAME_DEBUG_UPPER)
    get_property(AD_BUILDTYPENAME_DEV_UPPER GLOBAL PROPERTY AD_BUILDTYPENAME_DEV_UPPER)
    get_property(AD_BUILDTYPENAME_RELEASE_UPPER GLOBAL PROPERTY AD_BUILDTYPENAME_RELEASE_UPPER)

    get_property(AD_DEV_BUILD_LINKERFLAGS GLOBAL PROPERTY AD_DEV_BUILD_LINKERFLAGS)

    get_property(AD_CMAKE_RELPATH GLOBAL PROPERTY AD_CMAKE_RELPATH)
    get_property(AD_CMAKE_RELPATH_CODE GLOBAL PROPERTY AD_CMAKE_RELPATH_CODE)
    get_property(AD_CONFIG_PATH_7ZA GLOBAL PROPERTY AD_CONFIG_PATH_7ZA)

endmacro()

# #####################################
# ## ad_set_target_output_dirs(<target> <lib-output-dir> <dll-output-dir>)
# #####################################
function(ad_set_target_output_dirs TARGET_NAME LIB_OUTPUT_DIR DLL_OUTPUT_DIR)
    ad_pull_all_vars()
    ad_pull_config_vars()

    set(SUB_DIR "")
    set(PLATFORM_PREFIX "")
    set(PLATFORM_POSTFIX "")
    set(ARCH "x${AD_CMAKE_ARCHITECTURE_POSTFIX}")

    if (AD_CMAKE_PLATFORM_WINDOWS_UWP)
        # UWP has deployment problems if all applications output to the same path.
        set(SUB_DIR "/${TARGET_NAME}")
        set(PLATFORM_PREFIX "uwp_")

        if (${ARCH} STREQUAL "x32")
            set(ARCH "x86")
        endif ()

        if (${ARCH} STREQUAL "xArm32")
            set(ARCH "arm")
        endif ()

        if (${ARCH} STREQUAL "xArm64")
            set(ARCH "arm64")
        endif ()

    elseif (AD_CMAKE_PLATFORM_WINDOWS_DESKTOP)
        set(PLATFORM_POSTFIX "_win10")

    elseif (AD_CMAKE_PLATFORM_EMSCRIPTEN)
        set(PLATFORM_POSTFIX "_wasm")

    elseif (AD_CMAKE_PLATFORM_ANDROID)
        set(PLATFORM_POSTFIX "_android")
    endif ()

    string(TOLOWER ${AD_CMAKE_GENERATOR_PREFIX} LOWER_GENERATOR_PREFIX)

    set(PRE_PATH "${AD_CMAKE_PLATFORM_PREFIX}${AD_CMAKE_GENERATOR_PREFIX}${AD_CMAKE_COMPILER_POSTFIX}")
    set(OUTPUT_DEBUG "${PRE_PATH}${AD_BUILDTYPENAME_DEBUG}${AD_CMAKE_ARCHITECTURE_POSTFIX}${SUB_DIR}")
    set(OUTPUT_RELEASE "${PRE_PATH}${AD_BUILDTYPENAME_RELEASE}${AD_CMAKE_ARCHITECTURE_POSTFIX}${SUB_DIR}")
    set(OUTPUT_DEV "${PRE_PATH}${AD_BUILDTYPENAME_DEV}${AD_CMAKE_ARCHITECTURE_POSTFIX}${SUB_DIR}")

    set(OUTPUT_DLL_DEBUG "${DLL_OUTPUT_DIR}/${OUTPUT_DEBUG}")
    set(OUTPUT_LIB_DEBUG "${LIB_OUTPUT_DIR}/${OUTPUT_DEBUG}")

    set(OUTPUT_DLL_RELEASE "${DLL_OUTPUT_DIR}/${OUTPUT_RELEASE}")
    set(OUTPUT_LIB_RELEASE "${LIB_OUTPUT_DIR}/${OUTPUT_RELEASE}")

    set(OUTPUT_DLL_DEV "${DLL_OUTPUT_DIR}/${OUTPUT_DEV}")
    set(OUTPUT_LIB_DEV "${LIB_OUTPUT_DIR}/${OUTPUT_DEV}")

    # If we can't use generator expressions the non-generator expression version of the
    # output directory should point to the version matching CMAKE_BUILD_TYPE. This is the case for
    # add_custom_command BYPRODUCTS for example needed by Ninja.
    if ("${CMAKE_BUILD_TYPE}" STREQUAL ${AD_BUILDTYPENAME_DEBUG})
        set_target_properties(${TARGET_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DLL_DEBUG}"
                LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DLL_DEBUG}"
                ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_LIB_DEBUG}"
        )
    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL ${AD_BUILDTYPENAME_RELEASE})
        set_target_properties(${TARGET_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DLL_RELEASE}"
                LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DLL_RELEASE}"
                ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_LIB_RELEASE}"
        )
    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL ${AD_BUILDTYPENAME_DEV})
        set_target_properties(${TARGET_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DLL_DEV}"
                LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DLL_DEV}"
                ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_LIB_DEV}"
        )
    else ()
        message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'")
    endif ()

    set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${AD_BUILDTYPENAME_DEBUG_UPPER} "${OUTPUT_DLL_DEBUG}"
            LIBRARY_OUTPUT_DIRECTORY_${AD_BUILDTYPENAME_DEBUG_UPPER} "${OUTPUT_DLL_DEBUG}"
            ARCHIVE_OUTPUT_DIRECTORY_${AD_BUILDTYPENAME_DEBUG_UPPER} "${OUTPUT_LIB_DEBUG}"
    )

    set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${AD_BUILDTYPENAME_RELEASE_UPPER} "${OUTPUT_DLL_RELEASE}"
            LIBRARY_OUTPUT_DIRECTORY_${AD_BUILDTYPENAME_RELEASE_UPPER} "${OUTPUT_DLL_RELEASE}"
            ARCHIVE_OUTPUT_DIRECTORY_${AD_BUILDTYPENAME_RELEASE_UPPER} "${OUTPUT_LIB_RELEASE}"
    )

    set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${AD_BUILDTYPENAME_DEV_UPPER} "${OUTPUT_DLL_DEV}"
            LIBRARY_OUTPUT_DIRECTORY_${AD_BUILDTYPENAME_DEV_UPPER} "${OUTPUT_DLL_DEV}"
            ARCHIVE_OUTPUT_DIRECTORY_${AD_BUILDTYPENAME_DEV_UPPER} "${OUTPUT_LIB_DEV}"
    )
endfunction()

# #####################################
# ## ad_set_default_target_output_dirs(<target>)
# #####################################
function(ad_set_default_target_output_dirs TARGET_NAME)
    ad_set_target_output_dirs("${TARGET_NAME}" "${AD_OUTPUT_DIRECTORY_LIB}" "${AD_OUTPUT_DIRECTORY_DLL}")
endfunction()

# #####################################
# ## ad_write_configuration_txt()
# #####################################
function(ad_write_configuration_txt)
    # Clear Targets.txt and Tests.txt
    file(WRITE ${CMAKE_BINARY_DIR}/Targets.txt "")
    file(WRITE ${CMAKE_BINARY_DIR}/Tests.txt "")

    ad_pull_all_vars()
    ad_pull_config_vars()

    # Write configuration to file, as this is done at configure time we must pin the configuration in place (Dev is used because all build machines use this).
    file(WRITE ${CMAKE_BINARY_DIR}/Configuration.txt "")
    set(CONFIGURATION_DESC "${AD_CMAKE_PLATFORM_PREFIX}${AD_CMAKE_GENERATOR_PREFIX}${AD_CMAKE_COMPILER_POSTFIX}${AD_BUILDTYPENAME_DEV}${AD_CMAKE_ARCHITECTURE_POSTFIX}")
    file(APPEND ${CMAKE_BINARY_DIR}/Configuration.txt ${CONFIGURATION_DESC})
endfunction()

# #####################################
# ## ad_add_target_folder_as_include_dir(<target> <path-to-target>)
# #####################################
function(ad_add_target_folder_as_include_dir TARGET_NAME TARGET_FOLDER)
    get_filename_component(PARENT_DIR ${TARGET_FOLDER} DIRECTORY)

    # target_include_directories(${TARGET_NAME} PRIVATE "${TARGET_FOLDER}")
    target_include_directories(${TARGET_NAME} PUBLIC "${PARENT_DIR}")
endfunction()

# #####################################
# ## ad_set_common_target_definitions(<target>)
# #####################################
function(ad_set_common_target_definitions TARGET_NAME)
    ad_pull_all_vars()

    # set the BUILDSYSTEM_COMPILE_ENGINE_AS_DLL definition
    if (AD_COMPILE_MANAGER_AS_DLL)
        target_compile_definitions(${TARGET_NAME} PUBLIC BUILDSYSTEM_COMPILE_ENGINE_AS_DLL)
    endif ()

    target_compile_definitions(${TARGET_NAME} PRIVATE BUILDSYSTEM_SDKVERSION_MAJOR="${AD_CMAKE_SDKVERSION_MAJOR}")
    target_compile_definitions(${TARGET_NAME} PRIVATE BUILDSYSTEM_SDKVERSION_MINOR="${AD_CMAKE_SDKVERSION_MINOR}")
    target_compile_definitions(${TARGET_NAME} PRIVATE BUILDSYSTEM_SDKVERSION_PATCH="${AD_CMAKE_SDKVERSION_PATCH}")

    # set the BUILDSYSTEM_BUILDTYPE definition
    target_compile_definitions(${TARGET_NAME} PRIVATE BUILDSYSTEM_BUILDTYPE="${AD_CMAKE_GENERATOR_CONFIGURATION}")
    target_compile_definitions(${TARGET_NAME} PRIVATE BUILDSYSTEM_BUILDTYPE_${AD_CMAKE_GENERATOR_CONFIGURATION})

    # set the BUILDSYSTEM_BUILDING_XYZ_LIB definition
    string(TOUPPER ${TARGET_NAME} PROJECT_NAME_UPPER)
    target_compile_definitions(${TARGET_NAME} PRIVATE BUILDSYSTEM_BUILDING_${PROJECT_NAME_UPPER}_LIB)


    # on Windows, make sure to use the Unicode API
    target_compile_definitions(${TARGET_NAME} PUBLIC UNICODE _UNICODE)
endfunction()

# #####################################
# ## ad_set_project_ide_folder(<target> <path-to-target>)
# #####################################
function(ad_set_project_ide_folder TARGET_NAME PROJECT_SOURCE_DIR)
    # globally enable sorting targets into folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    get_filename_component(PARENT_FOLDER ${PROJECT_SOURCE_DIR} PATH)
    get_filename_component(FOLDER_NAME ${PARENT_FOLDER} NAME)

    set(IDE_FOLDER "${FOLDER_NAME}")

    if (${PROJECT_SOURCE_DIR} MATCHES "${CMAKE_SOURCE_DIR}/")
        set(IDE_FOLDER "")
        string(REPLACE "${CMAKE_SOURCE_DIR}/" "" PARENT_FOLDER ${PROJECT_SOURCE_DIR})

        get_filename_component(PARENT_FOLDER "${PARENT_FOLDER}" PATH)
        get_filename_component(FOLDER_NAME "${PARENT_FOLDER}" NAME)

        get_filename_component(PARENT_FOLDER2 "${PARENT_FOLDER}" PATH)

        while (NOT ${PARENT_FOLDER2} STREQUAL "")
            set(IDE_FOLDER "${FOLDER_NAME}/${IDE_FOLDER}")

            get_filename_component(PARENT_FOLDER "${PARENT_FOLDER}" PATH)
            get_filename_component(FOLDER_NAME "${PARENT_FOLDER}" NAME)

            get_filename_component(PARENT_FOLDER2 "${PARENT_FOLDER}" PATH)
        endwhile ()
    endif ()

    get_property(AD_SUBMODULE_MODE GLOBAL PROPERTY AD_SUBMODULE_MODE)

    if (AD_SUBMODULE_MODE)
        set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER "wdEngine/${IDE_FOLDER}")
    else ()
        set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER ${IDE_FOLDER})
    endif ()
endfunction()

# #####################################
# ## ad_add_output_ad_prefix(<target>)
# #####################################
function(ad_add_output_ad_prefix TARGET_NAME)
    set_target_properties(${TARGET_NAME} PROPERTIES IMPORT_PREFIX "")
    set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")
endfunction()

# #####################################
# ## ad_set_library_properties(<target>)
# #####################################
function(ad_set_library_properties TARGET_NAME)
    ad_pull_all_vars()

    if (AD_CMAKE_PLATFORM_LINUX)
        # c = libc.so (the C standard library)
        # m = libm.so (the C standard library math portion)
        # pthread = libpthread.so (thread support)
        # rt = librt.so (compiler runtime functions)
        target_link_libraries(${TARGET_NAME} PRIVATE pthread rt c m)

        if (AD_CMAKE_COMPILER_GCC)
            # Workaround for: https://bugs.launchpad.net/ubuntu/+source/gcc-5/+bug/1568899
            target_link_libraries(${TARGET_NAME} PRIVATE -lgcc_s -lgcc)
        endif ()
    endif ()
endfunction()

# #####################################
# ## ad_set_application_properties(<target>)
# #####################################
function(ad_set_application_properties TARGET_NAME)
    ad_pull_all_vars()

    # We need to link against pthread and rt last or linker errors will occur.
    if (AD_CMAKE_PLATFORM_LINUX)
        target_link_libraries(${TARGET_NAME} PRIVATE pthread rt)
    endif ()
endfunction()

# #####################################
# ## ad_make_winmain_executable(<target>)
# #####################################
function(ad_make_winmain_executable TARGET_NAME)
    set_property(TARGET ${TARGET_NAME} PROPERTY WIN32_EXECUTABLE ON)
endfunction()

# #####################################
# ## ad_gather_subfolders(<abs-path-to-folder> <out-sub-folders>)
# #####################################
function(ad_gather_subfolders START_FOLDER RESULT_FOLDERS)
    set(ALL_FILES "")
    set(ALL_DIRS "")

    file(GLOB_RECURSE ALL_FILES RELATIVE "${START_FOLDER}" "${START_FOLDER}/*")

    foreach (FILE ${ALL_FILES})
        get_filename_component(FILE_PATH ${FILE} DIRECTORY)

        list(APPEND ALL_DIRS ${FILE_PATH})
    endforeach ()

    list(REMOVE_DUPLICATES ALL_DIRS)

    set(${RESULT_FOLDERS} ${ALL_DIRS} PARENT_SCOPE)
endfunction()

# #####################################
# ## ad_glob_source_files(<path-to-folder> <out-files>)
# #####################################
function(ad_glob_source_files ROOT_DIR RESULT_ALL_SOURCES)
    file(GLOB_RECURSE RELEVANT_FILES
            "${ROOT_DIR}/*.cpp"
            "${ROOT_DIR}/*.xaml.cs"
            "${ROOT_DIR}/*.xaml"
            "${ROOT_DIR}/*.cc"
            "${ROOT_DIR}/*.h"
            "${ROOT_DIR}/*.hpp"
            "${ROOT_DIR}/*.inl"
            "${ROOT_DIR}/*.c"
            "${ROOT_DIR}/*.cs"
            "${ROOT_DIR}/*.ui"
            "${ROOT_DIR}/*.qrc"
            "${ROOT_DIR}/*.def"
            "${ROOT_DIR}/*.ico"
            "${ROOT_DIR}/*.rc"
            "${ROOT_DIR}/*.s"
            "${ROOT_DIR}/*.cmake"
            "${ROOT_DIR}/*.natvis"
            "${ROOT_DIR}/CMakeLists.txt"
    )

    set(${RESULT_ALL_SOURCES} ${RELEVANT_FILES} PARENT_SCOPE)
endfunction()

# #####################################
# ## ad_add_all_subdirs()
# #####################################
function(ad_add_all_subdirs)
    # find all cmake files below this directory
    file(GLOB SUB_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/*/CMakeLists.txt")

    foreach (VAR ${SUB_DIRS})
        get_filename_component(RES ${VAR} DIRECTORY)

        add_subdirectory(${RES})
    endforeach ()
endfunction()

# #####################################
# ## ad_cmake_init()
# #####################################
macro(ad_cmake_init)
    ad_pull_all_vars()
endmacro()

# #####################################
# ## ad_requires(<variable>)
# #####################################
macro(ad_requires)
    if (${ARGC} EQUAL 0)
        return()
    endif ()

    set(ALL_ARGS "${ARGN}")

    foreach (arg IN LISTS ALL_ARGS)
        if (NOT ${arg})
            return()
        endif ()
    endforeach ()
endmacro()

# #####################################
# ## ad_requires_one_of(<variable1> (<variable2>) (<variable3>) ...)
# #####################################
macro(ad_requires_one_of)
    if (${ARGC} EQUAL 0)
        message(FATAL_ERROR "ad_requires_one_of needs at least one argument")
    endif ()

    set(ALL_ARGS "${ARGN}")

    set(VALID 0)

    foreach (arg IN LISTS ALL_ARGS)
        if (${arg})
            set(VALID 1)
        endif ()
    endforeach ()

    if (NOT VALID)
        return()
    endif ()
endmacro()

# #####################################
# ## ad_requires_windows()
# #####################################
macro(ad_requires_windows)
    ad_requires(AD_CMAKE_PLATFORM_WINDOWS)
endmacro()

# #####################################
# ## ad_requires_windows_desktop()
# #####################################
macro(ad_requires_windows_desktop)
    ad_requires(AD_CMAKE_PLATFORM_WINDOWS_DESKTOP)
endmacro()


# #####################################
# ## ad_add_external_folder(<project-number>)
# #####################################
function(ad_add_external_projects_folder PROJECT_NUMBER)
    set(CACHE_VAR_NAME "AD_EXTERNAL_PROJECT${PROJECT_NUMBER}")

    set(${CACHE_VAR_NAME} "" CACHE PATH "A folder outside the engine repository that should be parsed for CMakeLists.txt files to include projects into the engine solution.")

    set(CACHE_VAR_VALUE ${${CACHE_VAR_NAME}})

    if (NOT CACHE_VAR_VALUE)
        return()
    endif ()

    set_property(GLOBAL PROPERTY "GATHER_EXTERNAL_PROJECTS" TRUE)
    add_subdirectory(${CACHE_VAR_VALUE} "${CMAKE_BINARY_DIR}/ExternalProject${PROJECT_NUMBER}")
    set_property(GLOBAL PROPERTY "GATHER_EXTERNAL_PROJECTS" FALSE)
endfunction()

# #####################################
# ## ad_init_projects()
# #####################################
# By defining AD_SOURCE_DIR before calling this function
# you can change the location that will be scanned for projects.
function(ad_init_projects)
    # find all init.cmake files below this directory or the given source directory if any.
    if (AD_SOURCE_DIR)
        file(GLOB_RECURSE INIT_FILES "${AD_SOURCE_DIR}/init.cmake")
    else ()
        file(GLOB_RECURSE INIT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/init.cmake")
    endif ()

    foreach (INIT_FILE ${INIT_FILES})
        message(STATUS "Including '${INIT_FILE}'")
        include("${INIT_FILE}")
    endforeach ()
endfunction()

# #####################################
# ## ad_finalize_projects()
# #####################################
# By defining AD_SOURCE_DIR before calling this function
# you can change the location that will be scanned for projects.
function(ad_finalize_projects)
    # find all finalize.cmake files below this directory or the given source directory if any.
    if (AD_SOURCE_DIR)
        file(GLOB_RECURSE FINALIZE_FILES "${AD_SOURCE_DIR}/finalize.cmake")
    else ()
        file(GLOB_RECURSE FINALIZE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/finalize.cmake")
    endif ()

    # TODO: also finalize external projects
    foreach (FINALIZE_FILE ${FINALIZE_FILES})
        message(STATUS "Including '${FINALIZE_FILE}'")
        include("${FINALIZE_FILE}")
    endforeach ()
endfunction()

# #####################################
# ## ad_build_filter_init()
# #####################################

# The build filter is intended to only build a subset of wdEngine.
# The build filters are configured through cmake files in the 'BuildFilters' directory.
function(ad_build_filter_init)
    file(GLOB_RECURSE FILTER_FILES "${CMAKE_SOURCE_DIR}/${AD_SUBMODULE_PREFIX_PATH}/Code/BuildSystem/CMake/BuildFilters/*.BuildFilter")

    get_property(AD_BUILD_FILTER_NAMES GLOBAL PROPERTY AD_BUILD_FILTER_NAMES)

    foreach (VAR ${FILTER_FILES})
        cmake_path(GET VAR STEM FILTER_NAME)
        list(APPEND AD_BUILD_FILTER_NAMES "${FILTER_NAME}")

        message(STATUS "Reading build filter '${FILTER_NAME}'")
        include(${VAR})
    endforeach ()

    list(REMOVE_DUPLICATES AD_BUILD_FILTER_NAMES)
    set_property(GLOBAL PROPERTY AD_BUILD_FILTER_NAMES ${AD_BUILD_FILTER_NAMES})

    set(AD_BUILD_FILTER "Everything" CACHE STRING "Which projects to include in the solution.")

    get_property(AD_BUILD_FILTER_NAMES GLOBAL PROPERTY AD_BUILD_FILTER_NAMES)
    set_property(CACHE AD_BUILD_FILTER PROPERTY STRINGS ${AD_BUILD_FILTER_NAMES})
    set_property(GLOBAL PROPERTY AD_BUILD_FILTER_SELECTED ${AD_BUILD_FILTER})
endfunction()

# #####################################
# ## ad_project_build_filter_index(<PROJECT_NAME> <OUT_INDEX>)
# #####################################
function(ad_project_build_filter_index PROJECT_NAME OUT_INDEX)
    get_property(SELECTED_FILTER_NAME GLOBAL PROPERTY AD_BUILD_FILTER_SELECTED)
    set(FILTER_VAR_NAME "AD_BUILD_FILTER_${SELECTED_FILTER_NAME}")
    get_property(FILTER_PROJECTS GLOBAL PROPERTY ${FILTER_VAR_NAME})

    list(LENGTH FILTER_PROJECTS LIST_LENGTH)

    if (${LIST_LENGTH} GREATER 1)
        list(FIND FILTER_PROJECTS ${PROJECT_NAME} FOUND_INDEX)
        set(${OUT_INDEX} ${FOUND_INDEX} PARENT_SCOPE)
    else ()
        set(${OUT_INDEX} 0 PARENT_SCOPE)
    endif ()
endfunction()

# #####################################
# ## ad_apply_build_filter(<PROJECT_NAME>)
# #####################################
macro(ad_apply_build_filter PROJECT_NAME)
    ad_project_build_filter_index(${PROJECT_NAME} PROJECT_INDEX)

    if (${PROJECT_INDEX} EQUAL -1)
        get_property(SELECTED_FILTER_NAME GLOBAL PROPERTY AD_BUILD_FILTER_SELECTED)
        message(STATUS "Project '${PROJECT_NAME}' excluded by build filter '${SELECTED_FILTER_NAME}'.")
        return()
    endif ()
endmacro()

# #####################################
# ## ad_set_build_types()
# #####################################
function(ad_set_build_types)
    ad_pull_config_vars()

    set(CMAKE_CONFIGURATION_TYPES "${AD_BUILDTYPENAME_DEBUG};${AD_BUILDTYPENAME_DEV};${AD_BUILDTYPENAME_RELEASE}" CACHE STRING "" FORCE)

    if (AD_BUILDTYPE_ONLY)
        set(CMAKE_CONFIGURATION_TYPES "${AD_BUILDTYPE_ONLY}" CACHE STRING "" FORCE)
    endif ()

    set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "AD build config types" FORCE)

    set(CMAKE_BUILD_TYPE ${AD_BUILDTYPENAME_DEV} CACHE STRING "The default build type")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})

    set(CMAKE_EXE_LINKER_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER} ${CMAKE_EXE_LINKER_FLAGS_DEBUG} CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER} ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER} ${CMAKE_EXE_LINKER_FLAGS_RELEASE} CACHE STRING "" FORCE)

    set(CMAKE_SHARED_LINKER_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER} ${CMAKE_SHARED_LINKER_FLAGS_DEBUG} CACHE STRING "" FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER} ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER} ${CMAKE_SHARED_LINKER_FLAGS_RELEASE} CACHE STRING "" FORCE)

    set(CMAKE_STATIC_LINKER_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER} ${CMAKE_STATIC_LINKER_FLAGS_DEBUG} CACHE STRING "" FORCE)
    set(CMAKE_STATIC_LINKER_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER} ${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
    set(CMAKE_STATIC_LINKER_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER} ${CMAKE_STATIC_LINKER_FLAGS_RELEASE} CACHE STRING "" FORCE)

    set(CMAKE_MODULE_LINKER_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER} ${CMAKE_MODULE_LINKER_FLAGS_DEBUG} CACHE STRING "" FORCE)
    set(CMAKE_MODULE_LINKER_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER} ${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
    set(CMAKE_MODULE_LINKER_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER} ${CMAKE_MODULE_LINKER_FLAGS_RELEASE} CACHE STRING "" FORCE)

    set(CMAKE_CXX_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER} ${CMAKE_CXX_FLAGS_DEBUG} CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER} ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER} ${CMAKE_CXX_FLAGS_RELEASE} CACHE STRING "" FORCE)

    set(CMAKE_C_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER} ${CMAKE_C_FLAGS_DEBUG} CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER} ${CMAKE_C_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER} ${CMAKE_C_FLAGS_RELEASE} CACHE STRING "" FORCE)

    set(CMAKE_CSharp_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER} ${CMAKE_CSharp_FLAGS_DEBUG} CACHE STRING "" FORCE)
    set(CMAKE_CSharp_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER} ${CMAKE_CSharp_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
    set(CMAKE_CSharp_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER} ${CMAKE_CSharp_FLAGS_RELEASE} CACHE STRING "" FORCE)

    set(CMAKE_RC_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER} ${CMAKE_RC_FLAGS_DEBUG} CACHE STRING "" FORCE)
    set(CMAKE_RC_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER} ${CMAKE_RC_FLAGS_RELWITHDEBINFO} CACHE STRING "" FORCE)
    set(CMAKE_RC_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER} ${CMAKE_RC_FLAGS_RELEASE} CACHE STRING "" FORCE)

    mark_as_advanced(FORCE CMAKE_EXE_LINKER_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER})
    mark_as_advanced(FORCE CMAKE_EXE_LINKER_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER})
    mark_as_advanced(FORCE CMAKE_EXE_LINKER_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER})
    mark_as_advanced(FORCE CMAKE_SHARED_LINKER_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER})
    mark_as_advanced(FORCE CMAKE_SHARED_LINKER_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER})
    mark_as_advanced(FORCE CMAKE_SHARED_LINKER_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER})
    mark_as_advanced(FORCE CMAKE_STATIC_LINKER_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER})
    mark_as_advanced(FORCE CMAKE_STATIC_LINKER_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER})
    mark_as_advanced(FORCE CMAKE_STATIC_LINKER_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER})
    mark_as_advanced(FORCE CMAKE_MODULE_LINKER_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER})
    mark_as_advanced(FORCE CMAKE_MODULE_LINKER_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER})
    mark_as_advanced(FORCE CMAKE_MODULE_LINKER_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER})
    mark_as_advanced(FORCE CMAKE_CXX_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER})
    mark_as_advanced(FORCE CMAKE_CXX_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER})
    mark_as_advanced(FORCE CMAKE_CXX_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER})
    mark_as_advanced(FORCE CMAKE_C_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER})
    mark_as_advanced(FORCE CMAKE_C_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER})
    mark_as_advanced(FORCE CMAKE_C_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER})
    mark_as_advanced(FORCE CMAKE_CSharp_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER})
    mark_as_advanced(FORCE CMAKE_CSharp_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER})
    mark_as_advanced(FORCE CMAKE_CSharp_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER})
    mark_as_advanced(FORCE CMAKE_RC_FLAGS_${AD_BUILDTYPENAME_DEBUG_UPPER})
    mark_as_advanced(FORCE CMAKE_RC_FLAGS_${AD_BUILDTYPENAME_DEV_UPPER})
    mark_as_advanced(FORCE CMAKE_RC_FLAGS_${AD_BUILDTYPENAME_RELEASE_UPPER})
endfunction()

# #####################################
# ## ad_download_and_extract(<url-to-download> <dest-folder-path> <dest-filename-without-extension>)
# #####################################
function(ad_download_and_extract URL DEST_FOLDER DEST_FILENAME)
    if (${URL} MATCHES ".tar.gz$")
        set(PKG_TYPE "tar.gz")
    else ()
        get_filename_component(PKG_TYPE ${URL} LAST_EXT)
    endif ()

    set(FULL_FILENAME "${DEST_FILENAME}.${PKG_TYPE}")
    set(PKG_FILE "${DEST_FOLDER}/${FULL_FILENAME}")
    set(EXTRACT_MARKER "${PKG_FILE}.extracted")

    if (EXISTS "${EXTRACT_MARKER}")
        return()
    endif ()

    # if the "URL" is actually a file path
    if (NOT "${URL}" MATCHES "http*")
        set(PKG_FILE "${URL}")
    endif ()

    if (NOT EXISTS "${PKG_FILE}")
        message(STATUS "Downloading '${FULL_FILENAME}'...")
        file(DOWNLOAD ${URL} "${PKG_FILE}" SHOW_PROGRESS STATUS DOWNLOAD_STATUS)

        list(GET DOWNLOAD_STATUS 0 DOWNLOAD_STATUS_CODE)

        if (NOT DOWNLOAD_STATUS_CODE EQUAL 0)
            message(FATAL_ERROR "Download failed: ${DOWNLOAD_STATUS}")
            return()
        endif ()
    endif ()

    ad_pull_config_vars()

    message(STATUS "Extracting '${FULL_FILENAME}'...")

    if (${PKG_TYPE} MATCHES "7z")
        execute_process(COMMAND "${AD_CONFIG_PATH_7ZA}"
                x "${PKG_FILE}"
                -aoa
                WORKING_DIRECTORY "${DEST_FOLDER}"
                COMMAND_ERROR_IS_FATAL ANY
                RESULT_VARIABLE CMD_STATUS)

    else ()
        execute_process(COMMAND ${CMAKE_COMMAND}
                -E tar -xf "${PKG_FILE}"
                WORKING_DIRECTORY "${DEST_FOLDER}"
                COMMAND_ERROR_IS_FATAL ANY
                RESULT_VARIABLE CMD_STATUS)
    endif ()

    if (NOT CMD_STATUS EQUAL 0)
        message(FATAL_ERROR "Extracting package '${FULL_FILENAME}' failed.")
        return()
    endif ()

    file(TOUCH ${EXTRACT_MARKER})
endfunction()
