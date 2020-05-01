#pragma once

#include "../dshowcapture.hpp"
#include "dshow-base.hpp"
#include "json.hpp"

namespace DShow {
    void SetDeviceSettings(IEnumMoniker *enumMoniker, nlohmann::json &camSettings);
    void GetDeviceSettings(IEnumMoniker *enumMoniker, nlohmann::json &camSettings);
    void LoadSettingsFromFile(const std::string& filePath);
    void SaveSettingsToFile(const std::string& filePath);
    void SetVideoProcSetting(IAMVideoProcAmp *videoProcSettings, nlohmann::detail::iter_impl<nlohmann::json> &settings, tagVideoProcAmpProperty videoProcProperty,
            const std::string& settingName, const std::string& flagName);
    void SetCamControlSetting(IAMCameraControl *camControlSettings, nlohmann::detail::iter_impl<nlohmann::json> &settings, tagCameraControlProperty camControlProperty,
            const std::string& settingName, const std::string& flagName);
};
