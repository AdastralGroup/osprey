#include <iostream>

#include <kachemak/kachemak.hpp>


int main()
{
	Kachemak kachemak = Kachemak("C:\\Program Files (x86)\\Steam\\steamapps\\sourcemods\\", "tf2classic");
	printf("Latest version: %s - Installed version: %s\n", kachemak.GetLatestVersion().value().szVersion.c_str(), kachemak.GetInstalledVersion().c_str());

	kachemak.Update();
	return 0;
}
