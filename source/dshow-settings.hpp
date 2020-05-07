#pragma once

#include "../dshowcapture.hpp"
#include "dshow-base.hpp"
#include <jansson.h>

namespace DShow {
    void SetDeviceSettings(IEnumMoniker *enumMoniker, json_t *devices);
    void GetDeviceSettings(IEnumMoniker *enumMoniker, json_t *device);
    void LoadSettingsFromFile(const std::string& filePath);
    void SaveSettingsToFile(const std::string& filePath);
    void SetVideoProcSetting(IAMVideoProcAmp *videoProcSettings, json_t *device,
	    tagVideoProcAmpProperty videoProcProperty,
            const std::string& settingName, const std::string& flagName);
    void SetCamControlSetting(IAMCameraControl *camControlSettings, json_t *device,
	    tagCameraControlProperty camControlProperty,
            const std::string& settingName, const std::string& flagName);
};
