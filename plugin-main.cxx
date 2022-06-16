#include <obs-module.h>

#include "plugin.hxx"

#include <iostream>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-midi-miem", "en-US")
bool obs_module_load(void)
{	
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	setlocale(LC_ALL, "Russian");
	
	Plugin::getInstance();

	blog(LOG_INFO, "obs-midi-miem plugin loaded successfully");
	return true;
}

void obs_module_unload()
{
	blog(LOG_INFO, "obs-midi-miem plugin unloaded");
}
