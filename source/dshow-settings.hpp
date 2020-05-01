#pragma once

#include "../dshowcapture.hpp"
#include "dshow-base.hpp"
#include "external/json.hpp"
#include <vector>

namespace DShow {
    void SetDeviceSettings(IEnumMoniker *enumMoniker, nlohmann::json &camSettings);
    void GetDeviceSettings(IEnumMoniker *enumMoniker, nlohmann::json &camSettings);

    void LoadSettingsFromFile(const std::string& filePath);
    void SaveSettingsToFile(const std::string& filePath);
};
