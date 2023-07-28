#include <AdastralManager/External/ADRegisterProject.h>
#include <xtree>

adastral::ADProjectRegister::ADProjectRegister()
{
	_classsingleton.initialize_class();
}

adastral::ADProjectRegister::~ADProjectRegister()
{
	_registeredprojects.clear();
}

void adastral::ADProjectRegister::KillClasses(GDExtensionInitializationLevel a)
{
}