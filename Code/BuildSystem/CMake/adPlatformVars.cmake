# Cmake variables which are platform dependent

# #####################################
# ## General settings
# #####################################
if(AD_CMAKE_PLATFORM_WINDOWS OR AD_CMAKE_PLATFORM_LINUX)
	set(AD_COMPILE_MANAGER_AS_DLL OFF BOOL "Whether to compile the code as a shared libraries (DLL).")
else()
	unset(AD_COMPILE_MANAGER_AS_DLL CACHE)
endif()

