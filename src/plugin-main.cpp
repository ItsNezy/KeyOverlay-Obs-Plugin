#include <obs-module.h>
#include "keyoverlay-plugin.hpp"

OBS_DECLARE_MODULE()

const char* obs_module_name(void) {
    return "KeyOverlay";
}

const char* obs_module_description(void) {
    return "Global keyboard overlay for OBS";
}

bool obs_module_load(void) {
    blog(LOG_INFO, "[KeyOverlay] Plugin loading...");
    return KeyOverlayPlugin::getInstance().init();
}

void obs_module_unload(void) {
    KeyOverlayPlugin::getInstance().shutdown();
    blog(LOG_INFO, "[KeyOverlay] Plugin unloaded.");
}
