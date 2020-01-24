#include <mutex>
#include <fstream>
#include <sstream>
#include <comdef.h>
#include "dshow-settings.hpp"

namespace DShow {
    void SetDeviceSettings(IEnumMoniker *enumMoniker, SettingsContainer& settings) {
        ComPtr<IMoniker> moniker;
        ComPtr<IAMVideoProcAmp> videoProcSettings;
        ComPtr<IAMCameraControl> camControlSettings;
        ComPtr<IPropertyBag> propertyBag;
        HRESULT hr;

        if (enumMoniker == nullptr) {
            return;
        }

        while (enumMoniker->Next(1, &moniker, nullptr) == S_OK) {
            hr = moniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, (void **)&propertyBag);
            if (FAILED(hr)) {
                continue;
            }

            VARIANT path, name;
            path.vt = VT_BSTR;
            name.vt = VT_BSTR;

            hr = propertyBag->Read(L"FriendlyName", &name, nullptr);
            if (SUCCEEDED(hr)) {
                const std::string nameStr(_bstr_t(name.bstrVal, true));
                auto settingsPair = settings.find(nameStr);
                if (settingsPair == settings.end()) {
                    continue;
                }

                hr = propertyBag->Read(L"DevicePath", &path, nullptr);
                if (SUCCEEDED(hr)) {
                    const std::string pathStr(_bstr_t(path.bstrVal, true));
                    if (settingsPair->second.devicePath != pathStr) {
                        continue;
                    }

                    hr = moniker->BindToObject(nullptr, nullptr, IID_IAMVideoProcAmp, (void **) &videoProcSettings);
                    if (SUCCEEDED(hr)) {
                        videoProcSettings->Set(VideoProcAmp_BacklightCompensation, settingsPair->second.backlightCompensation, settingsPair->second.backlightCompensationFlag);
                        videoProcSettings->Set(VideoProcAmp_Brightness, settingsPair->second.brightness, settingsPair->second.brightnessFlag);
                        videoProcSettings->Set(VideoProcAmp_ColorEnable, settingsPair->second.colorEnable, settingsPair->second.colorEnableFlag);
                        videoProcSettings->Set(VideoProcAmp_Contrast, settingsPair->second.contrast, settingsPair->second.contrastFlag);
                        videoProcSettings->Set(VideoProcAmp_Gain, settingsPair->second.gain, settingsPair->second.gainFlag);
                        videoProcSettings->Set(VideoProcAmp_Gamma, settingsPair->second.gamma, settingsPair->second.gammaFlag);
                        videoProcSettings->Set(VideoProcAmp_Hue, settingsPair->second.hue, settingsPair->second.hueFlag);
                        videoProcSettings->Set(VideoProcAmp_Saturation, settingsPair->second.saturation, settingsPair->second.saturationFlag);
                        videoProcSettings->Set(VideoProcAmp_Sharpness, settingsPair->second.sharpness, settingsPair->second.sharpnessFlag);
                        videoProcSettings->Set(VideoProcAmp_WhiteBalance, settingsPair->second.whiteBalance, settingsPair->second.whiteBalanceFlag);
                    }

                    hr = moniker->BindToObject(nullptr, nullptr, IID_IAMCameraControl, (void **) &camControlSettings);
                    if (SUCCEEDED(hr)) {
                        camControlSettings->Set(CameraControl_Exposure, settingsPair->second.exposure, settingsPair->second.exposureFlag);
                        camControlSettings->Set(CameraControl_Focus, settingsPair->second.focus, settingsPair->second.focusFlag);
                        camControlSettings->Set(CameraControl_Iris, settingsPair->second.iris, settingsPair->second.irisFlag);
                        camControlSettings->Set(CameraControl_Pan, settingsPair->second.pan, settingsPair->second.panFlag);
                        camControlSettings->Set(CameraControl_Roll, settingsPair->second.roll, settingsPair->second.rollFlag);
                        camControlSettings->Set(CameraControl_Tilt, settingsPair->second.tilt, settingsPair->second.tiltFlag);
                        camControlSettings->Set(CameraControl_Zoom, settingsPair->second.zoom, settingsPair->second.zoomFlag);
                    }
                }
            }
        }
    }

    void GetDeviceSettings(IEnumMoniker *enumMoniker, std::vector<std::string>& lines)
    {
        ComPtr<IMoniker> moniker;
        ComPtr<IAMVideoProcAmp> camVideoSettings;
        ComPtr<IAMCameraControl> camControlSettings;
        ComPtr<IPropertyBag> propertyBag;
        std::stringstream line;
        long value, flag;
        HRESULT hr;

        if (enumMoniker == nullptr) {
            return;
        }

        while (enumMoniker->Next(1, &moniker, nullptr) == S_OK) {
            hr = moniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, (void **)&propertyBag);

            if (SUCCEEDED(hr)) {
                VARIANT path, name;
                path.vt = VT_BSTR;
                name.vt = VT_BSTR;

                hr = propertyBag->Read(L"DevicePath", &path, nullptr);
                if (SUCCEEDED(hr)) {
                    const std::string pathStr(_bstr_t(path.bstrVal, true));
                    line << "DevicePath=" << pathStr << "\n";
                    lines.push_back(line.str());
                    line.str("");
                }

                hr = propertyBag->Read(L"FriendlyName", &name, nullptr);
                if (SUCCEEDED(hr)) {
                    const std::string nameStr(_bstr_t(name.bstrVal, true));
                    line << "DeviceName=" << nameStr << "\n";
                    lines.push_back(line.str());
                    line.str("");
                }

                // Getting VideoProcAmp settings
                hr = moniker->BindToObject(nullptr, nullptr, IID_IAMVideoProcAmp, (void**)&camVideoSettings);
                if (SUCCEEDED(hr)) {
                    hr = camVideoSettings->Get(VideoProcAmp_BacklightCompensation, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "BacklightCompensation=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "BacklightCompensation_flag=" << (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Brightness, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Brightness=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Brightness_flag=" << (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_ColorEnable, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "ColorEnable=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "ColorEnable_flag=" << (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Contrast, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Contrast=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Contrast_flag=" << (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Gain, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Gain=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Gain_flag=" << (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Gamma, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Gamma=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Gamma_flag=" << (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Hue, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Hue=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Hue_flag=" << (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Saturation, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Saturation=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Saturation_flag=" << (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Sharpness, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Sharpness=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Sharpness_flag=" << (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_WhiteBalance, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "WhiteBalance=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "WhiteBalance_flag=" << (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }
                }

                // Getting CameraControl settings
                hr = moniker->BindToObject(nullptr, nullptr, IID_IAMCameraControl, (void**)&camControlSettings);
                if (SUCCEEDED(hr)) {
                    hr = camControlSettings->Get(CameraControl_Exposure, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Exposure=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Exposure_flag=" << (flag > CameraControl_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camControlSettings->Get(CameraControl_Focus, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Focus=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Focus_flag=" << (flag > CameraControl_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camControlSettings->Get(CameraControl_Iris, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Iris=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Iris_flag=" << (flag > CameraControl_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camControlSettings->Get(CameraControl_Pan, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Pan=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Pan_flag=" << (flag > CameraControl_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camControlSettings->Get(CameraControl_Roll, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Roll=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Roll_flag=" << (flag > CameraControl_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camControlSettings->Get(CameraControl_Tilt, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Tilt=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Tilt_flag=" << (flag > CameraControl_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }

                    hr = camControlSettings->Get(CameraControl_Zoom, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        line << "Zoom=" << value << "\n";
                        lines.push_back(line.str());
                        line.str("");

                        line << "Zoom_flag=" << (flag > CameraControl_Flags_Auto ? "manual" : "auto") << "\n";
                        lines.push_back(line.str());
                        line.str("");
                    }
                }

                line.str("\n");
                lines.push_back(line.str());
            }
        }
    }

    static recursive_mutex settingsMutex;

    void LoadSettingsFromFile(const std::string& filePath, SettingsContainer& settings)
    {
        lock_guard<recursive_mutex> lock(settingsMutex);

        DeviceSettings deviceSettings;
        std::string line;
        std::ifstream file;

        file.open (filePath, ios::binary);

        if (file.is_open()) {
            while (std::getline(file, line)) {
                std::string key;
                std::string value;

                if (line.empty()) {
                    settings.insert(std::make_pair(deviceSettings.deviceName, deviceSettings));
                    continue;
                }

                std::istringstream attribute(line);
                std::getline(attribute, key, '=');
                std::getline(attribute, value);

                if (key == "DevicePath") {
                    deviceSettings.devicePath = value;
                }

                if (key == "DeviceName") {
                    deviceSettings.deviceName = value;
                }

                // VideoProcAmp settings
                if (key == "BacklightCompensation") {
                    deviceSettings.backlightCompensation = std::stol(value);
                }
                if (key == "BacklightCompensation_flag") {
                    deviceSettings.backlightCompensationFlag = (value == "manual" ? tagVideoProcAmpFlags::VideoProcAmp_Flags_Manual : tagVideoProcAmpFlags::VideoProcAmp_Flags_Auto);
                }

                if (key == "Brightness") {
                    deviceSettings.brightness = std::stol(value);
                }
                if (key == "Brightness_flag") {
                    deviceSettings.brightnessFlag = (value == "manual" ? tagVideoProcAmpFlags::VideoProcAmp_Flags_Manual : tagVideoProcAmpFlags::VideoProcAmp_Flags_Auto);
                }

                if (key == "ColorEnable") {
                    deviceSettings.colorEnable = std::stol(value);
                }
                if (key == "ColorEnable_flag") {
                    deviceSettings.colorEnableFlag = (value == "manual" ? tagVideoProcAmpFlags::VideoProcAmp_Flags_Manual : tagVideoProcAmpFlags::VideoProcAmp_Flags_Auto);
                }

                if (key == "Contrast") {
                    deviceSettings.contrast = std::stol(value);
                }
                if (key == "Contrast_flag") {
                    deviceSettings.contrastFlag = (value == "manual" ? tagVideoProcAmpFlags::VideoProcAmp_Flags_Manual : tagVideoProcAmpFlags::VideoProcAmp_Flags_Auto);
                }

                if (key == "Gain") {
                    deviceSettings.gain = std::stol(value);
                }
                if (key == "Gain_flag") {
                    deviceSettings.gainFlag = (value == "manual" ? tagVideoProcAmpFlags::VideoProcAmp_Flags_Manual : tagVideoProcAmpFlags::VideoProcAmp_Flags_Auto);
                }

                if (key == "Gamma") {
                    deviceSettings.gamma = std::stol(value);
                }
                if (key == "Gamma_flag") {
                    deviceSettings.gammaFlag = (value == "manual" ? tagVideoProcAmpFlags::VideoProcAmp_Flags_Manual : tagVideoProcAmpFlags::VideoProcAmp_Flags_Auto);
                }

                if (key == "Hue") {
                    deviceSettings.hue = std::stol(value);
                }
                if (key == "Hue_flag") {
                    deviceSettings.hueFlag = (value == "manual" ? tagVideoProcAmpFlags::VideoProcAmp_Flags_Manual : tagVideoProcAmpFlags::VideoProcAmp_Flags_Auto);
                }

                if (key == "Saturation") {
                    deviceSettings.saturation = std::stol(value);
                }
                if (key == "Saturation_flag") {
                    deviceSettings.saturationFlag = (value == "manual" ? tagVideoProcAmpFlags::VideoProcAmp_Flags_Manual : tagVideoProcAmpFlags::VideoProcAmp_Flags_Auto);
                }

                if (key == "Sharpness") {
                    deviceSettings.sharpness = std::stol(value);
                }
                if (key == "Sharpness_flag") {
                    deviceSettings.sharpnessFlag = (value == "manual" ? tagVideoProcAmpFlags::VideoProcAmp_Flags_Manual : tagVideoProcAmpFlags::VideoProcAmp_Flags_Auto);
                }

                if (key == "WhiteBalance") {
                    deviceSettings.backlightCompensation = std::stol(value);
                }
                if (key == "WhiteBalance_flag") {
                    deviceSettings.backlightCompensationFlag = (value == "manual" ? tagVideoProcAmpFlags::VideoProcAmp_Flags_Manual : tagVideoProcAmpFlags::VideoProcAmp_Flags_Auto);
                }

                // CameraControl settings
                if (key == "Exposure") {
                    deviceSettings.exposure = std::stol(value);
                }
                if (key == "Exposure_flag") {
                    deviceSettings.exposureFlag = (value == "manual" ? tagCameraControlFlags::CameraControl_Flags_Manual : tagCameraControlFlags::CameraControl_Flags_Auto);
                }

                if (key == "Focus") {
                    deviceSettings.focus = std::stol(value);
                }
                if (key == "Focus_flag") {
                    deviceSettings.focusFlag = (value == "manual" ? tagCameraControlFlags::CameraControl_Flags_Manual : tagCameraControlFlags::CameraControl_Flags_Auto);
                }

                if (key == "Iris") {
                    deviceSettings.iris = std::stol(value);
                }
                if (key == "Iris_flag") {
                    deviceSettings.irisFlag = (value == "manual" ? tagCameraControlFlags::CameraControl_Flags_Manual : tagCameraControlFlags::CameraControl_Flags_Auto);
                }

                if (key == "Pan") {
                    deviceSettings.pan = std::stol(value);
                }
                if (key == "Pan_flag") {
                    deviceSettings.panFlag = (value == "manual" ? tagCameraControlFlags::CameraControl_Flags_Manual : tagCameraControlFlags::CameraControl_Flags_Auto);
                }

                if (key == "Roll") {
                    deviceSettings.roll = std::stol(value);
                }
                if (key == "Roll_flag") {
                    deviceSettings.rollFlag = (value == "manual" ? tagCameraControlFlags::CameraControl_Flags_Manual : tagCameraControlFlags::CameraControl_Flags_Auto);
                }

                if (key == "Tilt") {
                    deviceSettings.tilt = std::stol(value);
                }
                if (key == "Tilt_flag") {
                    deviceSettings.tiltFlag = (value == "manual" ? tagCameraControlFlags::CameraControl_Flags_Manual : tagCameraControlFlags::CameraControl_Flags_Auto);
                }

                if (key == "Zoom") {
                    deviceSettings.zoom = std::stol(value);
                }
                if (key == "Zoom_flag") {
                    deviceSettings.zoomFlag = (value == "manual" ? tagCameraControlFlags::CameraControl_Flags_Manual : tagCameraControlFlags::CameraControl_Flags_Auto);
                }
            }

            file.close();
        }

        ComPtr<ICreateDevEnum> deviceEnum;
        HRESULT hr;

        hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr,
                              CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&deviceEnum);

        if (SUCCEEDED(hr)) {
            ComPtr<IEnumMoniker> enumMoniker;
            hr = deviceEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);

            if (SUCCEEDED(hr)) {
                SetDeviceSettings(enumMoniker, settings);
            }
        }
    }

    void SaveSettingsToFile(const std::string& filePath)
    {
        lock_guard<recursive_mutex> lock(settingsMutex);

        ComPtr<ICreateDevEnum> deviceEnum;
        HRESULT hr;

        hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr,
                CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&deviceEnum);

        if (SUCCEEDED(hr)) {
            ComPtr<IEnumMoniker> enumMoniker;
            hr = deviceEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);

            if (SUCCEEDED(hr)) {
                std::ofstream file;
                std::vector<std::string> lines;

                GetDeviceSettings(enumMoniker, lines);

                file.open(filePath, ios::trunc | ios::binary);

                if (file.is_open()) {
                    for (const auto &line : lines) {
                        file << line;
                    }

                    file.close();
                }
            }
        }
    }
};
