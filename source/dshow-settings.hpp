#pragma once

#include "../dshowcapture.hpp"
#include "dshow-base.hpp"
#include <vector>

namespace DShow {
    void SetDeviceSettings(IEnumMoniker *enumMoniker, SettingsContainer& settings);
    void GetDeviceSettings(IEnumMoniker *enumMoniker, std::vector<std::string>& lines);

    void LoadSettingsFromFile(const std::string& filePath, SettingsContainer& settings);
    void SaveSettingsToFile(const std::string& filePath);
};