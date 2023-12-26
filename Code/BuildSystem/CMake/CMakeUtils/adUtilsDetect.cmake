# #####################################
# ## ad_detect_project_name(<out-name>)
# #####################################

function(ad_detect_project_name OUT_NAME)
    # unfortunately this has to be known before the PROJECT command,
    # but platform and compiler settings are only detected by CMake AFTER the project command
    # CMAKE_GENERATOR is the only value available before that, so we have to regex this a bit to
    # generate a useful name
    # thus, only VS solutions currently get nice names
    cmake_path(IS_PREFIX CMAKE_SOURCE_DIR ${CMAKE_BINARY_DIR} NORMALIZE IS_IN_SOURCE_BUILD)

    get_filename_component(NAME_REPO ${CMAKE_SOURCE_DIR} NAME)
    get_filename_component(NAME_DEST ${CMAKE_BINARY_DIR} NAME)

    set(DETECTED_NAME "${NAME_REPO}")

    if (NOT ${NAME_REPO} STREQUAL ${NAME_DEST})
        set(DETECTED_NAME "${DETECTED_NAME}_${NAME_DEST}")
    endif ()

    set(${OUT_NAME} "${DETECTED_NAME}" PARENT_SCOPE)

    message(STATUS "Auto-detected solution name: ${DETECTED_NAME} (Generator = ${CMAKE_GENERATOR})")
endfunction()

# #####################################
# ## ad_detect_platform()
# #####################################
function(ad_detect_platform)
    get_property(PREFIX GLOBAL PROPERTY AD_CMAKE_PLATFORM_PREFIX)

    if (PREFIX)
        # has already run before and AD_CMAKE_PLATFORM_PREFIX is already set
        # message (STATUS "Redundant call to ad_detect_platform()")
        return()
    endif ()

    set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_PREFIX "")
    set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_WINDOWS OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_WINDOWS_DESKTOP OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_WINDOWS_7 OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_POSIX OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_OSX OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_LINUX OFF)

    message(STATUS "CMAKE_SYSTEM_NAME is '${CMAKE_SYSTEM_NAME}'")

    if (CMAKE_SYSTEM_NAME STREQUAL "Windows") # Desktop Windows
        message(STATUS "Platform is Windows (AD_CMAKE_PLATFORM_WINDOWS, AD_CMAKE_PLATFORM_WINDOWS_DESKTOP)")
        message(STATUS "CMAKE_SYSTEM_VERSION is ${CMAKE_SYSTEM_VERSION}")
        message(STATUS "CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION is ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")

        set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_WINDOWS ON)
        set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_WINDOWS_DESKTOP ON)

        set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_PREFIX "Win")

        if (${CMAKE_SYSTEM_VERSION} EQUAL 6.1)
            set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_WINDOWS_7 ON)
        endif ()

    elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CURRENT_OSX_VERSION) # OS X
        message(STATUS "Platform is OS X (AD_CMAKE_PLATFORM_OSX, AD_CMAKE_PLATFORM_POSIX)")

        set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_POSIX ON)
        set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_OSX ON)

        set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_PREFIX "Osx")

    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux") # Linux
        message(STATUS "Platform is Linux (AD_CMAKE_PLATFORM_LINUX, AD_CMAKE_PLATFORM_POSIX)")

        set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_POSIX ON)
        set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_LINUX ON)

        set_property(GLOBAL PROPERTY AD_CMAKE_PLATFORM_PREFIX "Linux")

    else ()
        message(FATAL_ERROR "Platform '${CMAKE_SYSTEM_NAME}' is not supported! Please extend ad_detect_platform().")
    endif ()

    get_property(AD_CMAKE_PLATFORM_WINDOWS GLOBAL PROPERTY AD_CMAKE_PLATFORM_WINDOWS)

    if (AD_CMAKE_PLATFORM_WINDOWS)
        if (CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
            set(AD_CMAKE_WINDOWS_SDK_VERSION ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION})
        else ()
            set(AD_CMAKE_WINDOWS_SDK_VERSION ${CMAKE_SYSTEM_VERSION})
            string(REGEX MATCHALL "\\." NUMBER_OF_DOTS "${AD_CMAKE_WINDOWS_SDK_VERSION}")
            list(LENGTH NUMBER_OF_DOTS NUMBER_OF_DOTS)

            if (NUMBER_OF_DOTS EQUAL 2)
                set(AD_CMAKE_WINDOWS_SDK_VERSION "${AD_CMAKE_WINDOWS_SDK_VERSION}.0")
            endif ()
        endif ()

        set_property(GLOBAL PROPERTY AD_CMAKE_WINDOWS_SDK_VERSION ${AD_CMAKE_WINDOWS_SDK_VERSION})
    endif ()
endfunction()

# #####################################
# ## ad_pull_platform_vars()
# #####################################
macro(ad_pull_platform_vars)
    ad_detect_platform()

    get_property(AD_CMAKE_PLATFORM_PREFIX GLOBAL PROPERTY AD_CMAKE_PLATFORM_PREFIX)
    get_property(AD_CMAKE_PLATFORM_WINDOWS GLOBAL PROPERTY AD_CMAKE_PLATFORM_WINDOWS)
    get_property(AD_CMAKE_PLATFORM_WINDOWS_DESKTOP GLOBAL PROPERTY AD_CMAKE_PLATFORM_WINDOWS_DESKTOP)
    get_property(AD_CMAKE_PLATFORM_WINDOWS_7 GLOBAL PROPERTY AD_CMAKE_PLATFORM_WINDOWS_7)
    get_property(AD_CMAKE_PLATFORM_POSIX GLOBAL PROPERTY AD_CMAKE_PLATFORM_POSIX)
    get_property(AD_CMAKE_PLATFORM_OSX GLOBAL PROPERTY AD_CMAKE_PLATFORM_OSX)
    get_property(AD_CMAKE_PLATFORM_LINUX GLOBAL PROPERTY AD_CMAKE_PLATFORM_LINUX)
    if (AD_CMAKE_PLATFORM_WINDOWS)
        get_property(AD_CMAKE_WINDOWS_SDK_VERSION GLOBAL PROPERTY AD_CMAKE_WINDOWS_SDK_VERSION)
    endif ()
endmacro()

# #####################################
# ## ad_detect_generator()
# #####################################
function(ad_detect_generator)
    get_property(PREFIX GLOBAL PROPERTY AD_CMAKE_GENERATOR_PREFIX)

    if (PREFIX)
        # has already run before and AD_CMAKE_GENERATOR_PREFIX is already set
        # message (STATUS "Redundant call to ad_detect_generator()")
        return()
    endif ()

    ad_pull_platform_vars()

    set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_PREFIX "")
    set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_CONFIGURATION "undefined")
    set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_MSVC OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_XCODE OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_MAKE OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_NINJA OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_INSIDE_VS OFF) # if cmake is called through the visual studio open folder workflow

    message(STATUS "CMAKE_VERSION is '${CMAKE_VERSION}'")
    message(STATUS "CMAKE_BUILD_TYPE is '${CMAKE_BUILD_TYPE}'")

    string(FIND ${CMAKE_VERSION} "MSVC" VERSION_CONTAINS_MSVC)

    if (${VERSION_CONTAINS_MSVC} GREATER -1)
        message(STATUS "CMake was called from Visual Studio Open Folder workflow")
        set_property(GLOBAL PROPERTY AD_CMAKE_INSIDE_VS ON)
    endif ()

    message(STATUS "CMAKE_GENERATOR is '${CMAKE_GENERATOR}'")

    if (AD_CMAKE_PLATFORM_WINDOWS) # Supported windows generators
        if (CMAKE_GENERATOR MATCHES "Visual Studio")
            # Visual Studio (All VS generators define MSVC)
            message(STATUS "Generator is MSVC (AD_CMAKE_GENERATOR_MSVC)")

            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_MSVC ON)
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_PREFIX "Vs")
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_CONFIGURATION $<CONFIGURATION>)
        elseif (CMAKE_GENERATOR MATCHES "Ninja") # Ninja makefiles. Only makefile format supported by Visual Studio Open Folder
            message(STATUS "Buildsystem is Ninja (AD_CMAKE_GENERATOR_NINJA)")

            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_NINJA ON)
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_PREFIX "Ninja")
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_CONFIGURATION ${CMAKE_BUILD_TYPE})
        else ()
            message(FATAL_ERROR "Generator '${CMAKE_GENERATOR}' is not supported on Windows! Please extend ad_detect_generator()")
        endif ()

    elseif (AD_CMAKE_PLATFORM_OSX) # Supported OSX generators
        if (CMAKE_GENERATOR MATCHES "Xcode") # XCODE
            message(STATUS "Buildsystem is Xcode (AD_CMAKE_GENERATOR_XCODE)")

            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_XCODE ON)
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_PREFIX "Xcode")
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_CONFIGURATION $<CONFIGURATION>)

        elseif (CMAKE_GENERATOR MATCHES "Unix Makefiles") # Unix Makefiles (for QtCreator etc.)
            message(STATUS "Buildsystem is Make (AD_CMAKE_GENERATOR_MAKE)")

            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_MAKE ON)
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_PREFIX "Make")
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_CONFIGURATION ${CMAKE_BUILD_TYPE})

        else ()
            message(FATAL_ERROR "Generator '${CMAKE_GENERATOR}' is not supported on OS X! Please extend ad_detect_generator()")
        endif ()

    elseif (AD_CMAKE_PLATFORM_LINUX)
        if (CMAKE_GENERATOR MATCHES "Unix Makefiles") # Unix Makefiles (for QtCreator etc.)
            message(STATUS "Buildsystem is Make (AD_CMAKE_GENERATOR_MAKE)")

            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_MAKE ON)
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_PREFIX "Make")
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_CONFIGURATION ${CMAKE_BUILD_TYPE})

        elseif (CMAKE_GENERATOR MATCHES "Ninja" OR CMAKE_GENERATOR MATCHES "Ninja Multi-Config")
            message(STATUS "Buildsystem is Ninja (AD_CMAKE_GENERATOR_NINJA)")

            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_NINJA ON)
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_PREFIX "Ninja")
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_CONFIGURATION ${CMAKE_BUILD_TYPE})
        else ()
            message(FATAL_ERROR "Generator '${CMAKE_GENERATOR}' is not supported on Linux! Please extend ad_detect_generator()")
        endif ()

    elseif (AD_CMAKE_PLATFORM_ANDROID)
        if (CMAKE_GENERATOR MATCHES "Ninja" OR CMAKE_GENERATOR MATCHES "Ninja Multi-Config")
            message(STATUS "Buildsystem is Ninja (AD_CMAKE_GENERATOR_NINJA)")

            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_NINJA ON)
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_PREFIX "Ninja")
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_CONFIGURATION ${CMAKE_BUILD_TYPE})

        else ()
            message(FATAL_ERROR "Generator '${CMAKE_GENERATOR}' is not supported on Android! Please extend ad_detect_generator()")
        endif ()

    elseif (AD_CMAKE_PLATFORM_EMSCRIPTEN)
        if (CMAKE_GENERATOR MATCHES "Ninja" OR CMAKE_GENERATOR MATCHES "Ninja Multi-Config")
            message(STATUS "Buildsystem is Ninja (AD_CMAKE_GENERATOR_NINJA)")

            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_NINJA ON)
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_PREFIX "Ninja")
            set_property(GLOBAL PROPERTY AD_CMAKE_GENERATOR_CONFIGURATION ${CMAKE_BUILD_TYPE})

        else ()
            message(FATAL_ERROR "Generator '${CMAKE_GENERATOR}' is not supported on Emscripten! Please extend ad_detect_generator()")
        endif ()

    else ()
        message(FATAL_ERROR "Platform '${CMAKE_SYSTEM_NAME}' has not set up the supported generators. Please extend ad_detect_generator()")
    endif ()
endfunction()

# #####################################
# ## ad_pull_generator_vars()
# #####################################
macro(ad_pull_generator_vars)
    ad_detect_generator()

    get_property(AD_CMAKE_GENERATOR_PREFIX GLOBAL PROPERTY AD_CMAKE_GENERATOR_PREFIX)
    get_property(AD_CMAKE_GENERATOR_CONFIGURATION GLOBAL PROPERTY AD_CMAKE_GENERATOR_CONFIGURATION)
    get_property(AD_CMAKE_GENERATOR_MSVC GLOBAL PROPERTY AD_CMAKE_GENERATOR_MSVC)
    get_property(AD_CMAKE_GENERATOR_XCODE GLOBAL PROPERTY AD_CMAKE_GENERATOR_XCODE)
    get_property(AD_CMAKE_GENERATOR_MAKE GLOBAL PROPERTY AD_CMAKE_GENERATOR_MAKE)
    get_property(AD_CMAKE_GENERATOR_NINJA GLOBAL PROPERTY AD_CMAKE_GENERATOR_NINJA)
    get_property(AD_CMAKE_INSIDE_VS GLOBAL PROPERTY AD_CMAKE_INSIDE_VS)
endmacro()

# #####################################
# ## ad_detect_compiler_and_architecture()
# #####################################
function(ad_detect_compiler_and_architecture)
    get_property(PREFIX GLOBAL PROPERTY AD_CMAKE_COMPILER_POSTFIX)

    if (PREFIX)
        # has already run before and AD_CMAKE_COMPILER_POSTFIX is already set
        # message (STATUS "Redundant call to ad_detect_compiler()")
        return()
    endif ()

    ad_pull_platform_vars()
    ad_pull_generator_vars()
    ad_pull_config_vars()
    get_property(GENERATOR_MSVC GLOBAL PROPERTY AD_CMAKE_GENERATOR_MSVC)

    set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_POSTFIX "")
    set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC_140 OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC_141 OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC_142 OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC_143 OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_CLANG OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_GCC OFF)

    set(FILE_TO_COMPILE "${CMAKE_SOURCE_DIR}/${AD_SUBMODULE_PREFIX_PATH}/${AD_CMAKE_RELPATH}/ProbingSrc/ArchitectureDetect.c")

    if (AD_SDK_DIR)
        set(FILE_TO_COMPILE "${AD_SDK_DIR}/${AD_CMAKE_RELPATH}/ProbingSrc/ArchitectureDetect.c")
    endif ()

    # Only compile the detect file if we don't have a cached result from the last run
    if ((NOT AD_DETECTED_COMPILER) OR (NOT AD_DETECTED_ARCH) OR (NOT AD_DETECTED_MSVC_VER))
        set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
        try_compile(COMPILE_RESULT
                ${CMAKE_CURRENT_BINARY_DIR}
                ${FILE_TO_COMPILE}
                OUTPUT_VARIABLE COMPILE_OUTPUT
        )

        if (NOT COMPILE_RESULT)
            message(FATAL_ERROR "Failed to detect compiler / target architecture. Compiler output: ${COMPILE_OUTPUT}")
        endif ()

        if (${COMPILE_OUTPUT} MATCHES "ARCH:'([^']*)'")
            set(AD_DETECTED_ARCH ${CMAKE_MATCH_1} CACHE INTERNAL "")
        else ()
            message(FATAL_ERROR "The compile test did not output the architecture. Compiler broken? Compiler output: ${COMPILE_OUTPUT}")
        endif ()

        if (${COMPILE_OUTPUT} MATCHES "COMPILER:'([^']*)'")
            set(AD_DETECTED_COMPILER ${CMAKE_MATCH_1} CACHE INTERNAL "")
        else ()
            message(FATAL_ERROR "The compile test did not output the compiler. Compiler broken? Compiler output: ${COMPILE_OUTPUT}")
        endif ()

        if (AD_DETECTED_COMPILER STREQUAL "msvc")
            if (${COMPILE_OUTPUT} MATCHES "MSC_VER:'([^']*)'")
                set(AD_DETECTED_MSVC_VER ${CMAKE_MATCH_1} CACHE INTERNAL "")
            else ()
                message(FATAL_ERROR "The compile test did not output the MSC_VER. Compiler broken? Compiler output: ${COMPILE_OUTPUT}")
            endif ()
        else ()
            set(AD_DETECTED_MSVC_VER "<NOT USING MSVC>" CACHE INTERNAL "")
        endif ()
    endif ()

    if (AD_DETECTED_COMPILER STREQUAL "msvc") # Visual Studio Compiler
        message(STATUS "Compiler is MSVC (AD_CMAKE_COMPILER_MSVC) version ${AD_DETECTED_MSVC_VER}")

        set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC ON)

        if (AD_DETECTED_MSVC_VER GREATER_EQUAL 1930)
            message(STATUS "Compiler is Visual Studio 2022 (AD_CMAKE_COMPILER_MSVC_143)")
            set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC_143 ON)
            set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_POSTFIX "2022")

        elseif (AD_DETECTED_MSVC_VER GREATER_EQUAL 1920)
            message(STATUS "Compiler is Visual Studio 2019 (AD_CMAKE_COMPILER_MSVC_142)")
            set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC_142 ON)
            set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_POSTFIX "2019")

        elseif (AD_DETECTED_MSVC_VER GREATER_EQUAL 1910)
            message(STATUS "Compiler is Visual Studio 2017 (AD_CMAKE_COMPILER_MSVC_141)")
            set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC_141 ON)
            set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_POSTFIX "2017")

        elseif (MSVC_VERSION GREATER_EQUAL 1900)
            message(STATUS "Compiler is Visual Studio 2015 (AD_CMAKE_COMPILER_MSVC_140)")
            set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC_140 ON)
            set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_POSTFIX "2015")

        else ()
            message(FATAL_ERROR "Compiler for generator '${CMAKE_GENERATOR}' is not supported on MSVC! Please extend ad_detect_compiler()")
        endif ()

    elseif (AD_DETECTED_COMPILER STREQUAL "clang")
        message(STATUS "Compiler is clang (AD_CMAKE_COMPILER_CLANG)")
        set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_CLANG ON)
        set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_POSTFIX "Clang")

    elseif (AD_DETECTED_COMPILER STREQUAL "gcc")
        message(STATUS "Compiler is gcc (AD_CMAKE_COMPILER_GCC)")
        set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_GCC ON)
        set_property(GLOBAL PROPERTY AD_CMAKE_COMPILER_POSTFIX "Gcc")

    else ()
        message(FATAL_ERROR "Unhandled compiler ${AD_DETECTED_COMPILER}")
    endif ()

    set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_POSTFIX "")
    set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_32BIT OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_64BIT OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_X86 OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_ARM OFF)
    set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_EMSCRIPTEN OFF)

    if (AD_DETECTED_ARCH STREQUAL "x86")
        message(STATUS "Architecture is X86 (AD_CMAKE_ARCHITECTURE_X86)")
        set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_X86 ON)

        message(STATUS "Architecture is 32-Bit (AD_CMAKE_ARCHITECTURE_32BIT)")
        set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_32BIT ON)

    elseif (AD_DETECTED_ARCH STREQUAL "x64")
        message(STATUS "Architecture is X86 (AD_CMAKE_ARCHITECTURE_X86)")
        set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_X86 ON)

        message(STATUS "Architecture is 64-Bit (AD_CMAKE_ARCHITECTURE_64BIT)")
        set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_64BIT ON)

    elseif (AD_DETECTED_ARCH STREQUAL "arm32")
        message(STATUS "Architecture is ARM (AD_CMAKE_ARCHITECTURE_ARM)")
        set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_ARM ON)

        message(STATUS "Architecture is 32-Bit (AD_CMAKE_ARCHITECTURE_32BIT)")
        set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_32BIT ON)

    elseif (AD_DETECTED_ARCH STREQUAL "arm64")
        message(STATUS "Architecture is ARM (AD_CMAKE_ARCHITECTURE_ARM)")
        set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_ARM ON)

        message(STATUS "Architecture is 64-Bit (AD_CMAKE_ARCHITECTURE_64BIT)")
        set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_64BIT ON)

    elseif (AD_DETECTED_ARCH STREQUAL "emscripten")
        message(STATUS "Architecture is WEBASSEMBLY (AD_CMAKE_ARCHITECTURE_WEBASSEMBLY)")
        set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_WEBASSEMBLY ON)

        if (CMAKE_SIZEOF_VOID_P EQUAL 8)
            message(STATUS "Architecture is 64-Bit (AD_CMAKE_ARCHITECTURE_64BIT)")
            set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_64BIT ON)
        else ()
            message(STATUS "Architecture is 32-Bit (AD_CMAKE_ARCHITECTURE_32BIT)")
            set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_32BIT ON)
        endif ()

    else ()
        message(FATAL_ERROR "Unhandled target architecture ${AD_DETECTED_ARCH}")
    endif ()

    get_property(AD_CMAKE_ARCHITECTURE_32BIT GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_32BIT)
    get_property(AD_CMAKE_ARCHITECTURE_ARM GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_ARM)

    if (AD_CMAKE_ARCHITECTURE_ARM)
        if (AD_CMAKE_ARCHITECTURE_32BIT)
            set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_POSTFIX "Arm32")
        else ()
            set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_POSTFIX "Arm64")
        endif ()
    else ()
        if (AD_CMAKE_ARCHITECTURE_32BIT)
            set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_POSTFIX "32")
        else ()
            set_property(GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_POSTFIX "64")
        endif ()
    endif ()
endfunction()

# #####################################
# ## ad_pull_compiler_vars()
# #####################################
macro(ad_pull_compiler_and_architecture_vars)
    ad_detect_compiler_and_architecture()

    get_property(AD_CMAKE_COMPILER_POSTFIX GLOBAL PROPERTY AD_CMAKE_COMPILER_POSTFIX)
    get_property(AD_CMAKE_COMPILER_MSVC GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC)
    get_property(AD_CMAKE_COMPILER_MSVC_140 GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC_140)
    get_property(AD_CMAKE_COMPILER_MSVC_141 GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC_141)
    get_property(AD_CMAKE_COMPILER_MSVC_142 GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC_142)
    get_property(AD_CMAKE_COMPILER_MSVC_143 GLOBAL PROPERTY AD_CMAKE_COMPILER_MSVC_143)
    get_property(AD_CMAKE_COMPILER_CLANG GLOBAL PROPERTY AD_CMAKE_COMPILER_CLANG)
    get_property(AD_CMAKE_COMPILER_GCC GLOBAL PROPERTY AD_CMAKE_COMPILER_GCC)

    get_property(AD_CMAKE_ARCHITECTURE_POSTFIX GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_POSTFIX)
    get_property(AD_CMAKE_ARCHITECTURE_32BIT GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_32BIT)
    get_property(AD_CMAKE_ARCHITECTURE_64BIT GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_64BIT)
    get_property(AD_CMAKE_ARCHITECTURE_X86 GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_X86)
    get_property(AD_CMAKE_ARCHITECTURE_ARM GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_ARM)
    get_property(AD_CMAKE_ARCHITECTURE_WEBASSEMBLY GLOBAL PROPERTY AD_CMAKE_ARCHITECTURE_WEBASSEMBLY)
endmacro()

# #####################################
# ## ad_pull_all_vars()
# #####################################
macro(ad_pull_all_vars)
    get_property(AD_SUBMODULE_PREFIX_PATH GLOBAL PROPERTY AD_SUBMODULE_PREFIX_PATH)

    ad_pull_version()
    ad_pull_compiler_and_architecture_vars()
    ad_pull_generator_vars()
    ad_pull_platform_vars()
endmacro()

# #####################################
# ## ad_get_version(<VERSIONFILE> <OUT_MAJOR> <OUT_MINOR> <OUT_PATCH>)
# #####################################
function(ad_get_version VERSIONFILE OUT_MAJOR OUT_MINOR OUT_PATCH)
    file(READ ${VERSIONFILE} VERSION_STRING)

    string(STRIP ${VERSION_STRING} VERSION_STRING)

    if (VERSION_STRING MATCHES "([0-9]+).([0-9]+).([0-9+])")
        STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+" "\\1" VERSION_MAJOR "${VERSION_STRING}")
        STRING(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+" "\\1" VERSION_MINOR "${VERSION_STRING}")
        STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+)" "\\1" VERSION_PATCH "${VERSION_STRING}")

        string(STRIP ${VERSION_MAJOR} VERSION_MAJOR)
        string(STRIP ${VERSION_MINOR} VERSION_MINOR)
        string(STRIP ${VERSION_PATCH} VERSION_PATCH)

        set(${OUT_MAJOR} ${VERSION_MAJOR} PARENT_SCOPE)
        set(${OUT_MINOR} ${VERSION_MINOR} PARENT_SCOPE)
        set(${OUT_PATCH} ${VERSION_PATCH} PARENT_SCOPE)

    else ()
        message(FATAL_ERROR "Invalid version string '${VERSION_STRING}'")
    endif ()
endfunction()

# #####################################
# ## ad_detect_version()
# #####################################
function(ad_detect_version)
    get_property(VERSION_MAJOR GLOBAL PROPERTY AD_CMAKE_SDKVERSION_MAJOR)

    if (VERSION_MAJOR)
        # has already run before and AD_CMAKE_SDKVERSION_MAJOR is already set
        return()
    endif ()

    ad_get_version("${CMAKE_SOURCE_DIR}/${AD_SUBMODULE_PREFIX_PATH}/version.txt" VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

    set_property(GLOBAL PROPERTY AD_CMAKE_SDKVERSION_MAJOR "${VERSION_MAJOR}")
    set_property(GLOBAL PROPERTY AD_CMAKE_SDKVERSION_MINOR "${VERSION_MINOR}")
    set_property(GLOBAL PROPERTY AD_CMAKE_SDKVERSION_PATCH "${VERSION_PATCH}")

    message(STATUS "SDK version: Major = '${VERSION_MAJOR}', Minor = '${VERSION_MINOR}', Patch = '${VERSION_PATCH}'")
endfunction()

# #####################################
# ## ad_pull_version()
# #####################################
macro(ad_pull_version)
    ad_detect_version()

    get_property(AD_CMAKE_SDKVERSION_MAJOR GLOBAL PROPERTY AD_CMAKE_SDKVERSION_MAJOR)
    get_property(AD_CMAKE_SDKVERSION_MINOR GLOBAL PROPERTY AD_CMAKE_SDKVERSION_MINOR)
    get_property(AD_CMAKE_SDKVERSION_PATCH GLOBAL PROPERTY AD_CMAKE_SDKVERSION_PATCH)
endmacro()