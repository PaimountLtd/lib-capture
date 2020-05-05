#include <mutex>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <comdef.h>
#include "dshow-settings.hpp"

namespace DShow {
    void SetDeviceSettings(IEnumMoniker *enumMoniker, nlohmann::json &camSettings) {
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

                hr = propertyBag->Read(L"DevicePath", &path, nullptr);
                if (SUCCEEDED(hr)) {
                    const std::string pathStr(_bstr_t(path.bstrVal, true));

                    auto settings = std::find_if(camSettings.begin(), camSettings.end(), [nameStr, pathStr](const nlohmann::json& j) {
                        return j.is_object() && j.value("DevicePath", "") == pathStr && j.value("DeviceName", "") == nameStr;
                    });

                    hr = moniker->BindToObject(nullptr, nullptr, IID_IAMVideoProcAmp, (void **) &videoProcSettings);
                    if (SUCCEEDED(hr) && settings != camSettings.end()) {
                        SetVideoProcSetting(videoProcSettings, settings, VideoProcAmp_BacklightCompensation, "BacklightCompensation", "BacklightCompensation_flag");
                        SetVideoProcSetting(videoProcSettings, settings, VideoProcAmp_Brightness, "Brightness", "Brightness_flag");
                        SetVideoProcSetting(videoProcSettings, settings, VideoProcAmp_ColorEnable, "ColorEnable", "ColorEnable_flag");
                        SetVideoProcSetting(videoProcSettings, settings, VideoProcAmp_Contrast, "Contrast", "Contrast_flag");
                        SetVideoProcSetting(videoProcSettings, settings, VideoProcAmp_Gain, "Gain", "Gain_flag");
                        SetVideoProcSetting(videoProcSettings, settings, VideoProcAmp_Gamma, "Gamma", "Gamma_flag");
                        SetVideoProcSetting(videoProcSettings, settings, VideoProcAmp_Hue, "Hue", "Hue_flag");
                        SetVideoProcSetting(videoProcSettings, settings, VideoProcAmp_Saturation, "Saturation", "Saturation_flag");
                        SetVideoProcSetting(videoProcSettings, settings, VideoProcAmp_Sharpness, "Sharpness", "Sharpness_flag");
                        SetVideoProcSetting(videoProcSettings, settings, VideoProcAmp_WhiteBalance, "WhiteBalance", "WhiteBalance_flag");
                    }

                    hr = moniker->BindToObject(nullptr, nullptr, IID_IAMCameraControl, (void **) &camControlSettings);
                    if (SUCCEEDED(hr)) {
                        SetCamControlSetting(camControlSettings, settings, CameraControl_Exposure, "Exposure", "Exposure_flag");
                        SetCamControlSetting(camControlSettings, settings, CameraControl_Focus, "Focus", "Focus_flag");
                        SetCamControlSetting(camControlSettings, settings, CameraControl_Iris, "Iris", "Iris_flag");
                        SetCamControlSetting(camControlSettings, settings, CameraControl_Pan, "Pan", "Pan_flag");
                        SetCamControlSetting(camControlSettings, settings, CameraControl_Roll, "Roll", "Roll_flag");
                        SetCamControlSetting(camControlSettings, settings, CameraControl_Tilt, "Tilt", "Tilt_flag");
                        SetCamControlSetting(camControlSettings, settings, CameraControl_Zoom, "Zoom", "Zoom_flag");
                    }
                }
            }
        }
    }

    void GetDeviceSettings(IEnumMoniker *enumMoniker, nlohmann::json &camSettings)
    {
        ComPtr<IMoniker> moniker;
        ComPtr<IAMVideoProcAmp> camVideoSettings;
        ComPtr<IAMCameraControl> camControlSettings;
        ComPtr<IPropertyBag> propertyBag;
        long value, flag;
        nlohmann::json settings;
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
                    settings.emplace("DevicePath", pathStr);
                }

                hr = propertyBag->Read(L"FriendlyName", &name, nullptr);
                if (SUCCEEDED(hr)) {
                    const std::string nameStr(_bstr_t(name.bstrVal, true));
                    settings.emplace("DeviceName", nameStr);
                }

                // Getting VideoProcAmp settings
                hr = moniker->BindToObject(nullptr, nullptr, IID_IAMVideoProcAmp, (void**)&camVideoSettings);
                if (SUCCEEDED(hr)) {
                    hr = camVideoSettings->Get(VideoProcAmp_BacklightCompensation, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("BacklightCompensation", value);
                        settings.emplace("BacklightCompensation_flag", (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Brightness, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Brightness", value);
                        settings.emplace("Brightness_flag", (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_ColorEnable, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("ColorEnable", value);
                        settings.emplace("ColorEnable_flag", (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Contrast, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Contrast", value);
                        settings.emplace("Contrast_flag", (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Gain, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Gain", value);
                        settings.emplace("Gain_flag", (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Gamma, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Gamma", value);
                        settings.emplace("Gamma_flag", (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Hue, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Hue", value);
                        settings.emplace("Hue_flag", (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Saturation, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Saturation", value);
                        settings.emplace("Saturation_flag", (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Sharpness, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Sharpness", value);
                        settings.emplace("Sharpness_flag", (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_WhiteBalance, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("WhiteBalance", value);

                        // Some devices return flag value 3 when the flag is auto
                        if (flag == 3) {
                            settings.emplace("WhiteBalance_flag", "auto");
                        } else {
                            settings.emplace("WhiteBalance_flag", (flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                        }
                    }
                }

                // Getting CameraControl settings
                hr = moniker->BindToObject(nullptr, nullptr, IID_IAMCameraControl, (void**)&camControlSettings);
                if (SUCCEEDED(hr)) {
                    hr = camControlSettings->Get(CameraControl_Exposure, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Exposure", value);
                        settings.emplace("Exposure_flag", (flag > CameraControl_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camControlSettings->Get(CameraControl_Focus, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Focus", value);
                        settings.emplace("Focus_flag", (flag > CameraControl_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camControlSettings->Get(CameraControl_Iris, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Iris", value);
                        settings.emplace("Iris_flag", (flag > CameraControl_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camControlSettings->Get(CameraControl_Pan, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Pan", value);
                        settings.emplace("Pan_flag", (flag > CameraControl_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camControlSettings->Get(CameraControl_Roll, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Roll", value);
                        settings.emplace("Roll_flag", (flag > CameraControl_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camControlSettings->Get(CameraControl_Tilt, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Tilt", value);
                        settings.emplace("Tilt_flag", (flag > CameraControl_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camControlSettings->Get(CameraControl_Zoom, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        settings.emplace("Zoom", value);
                        settings.emplace("Zoom_flag", (flag > CameraControl_Flags_Auto ? "manual" : "auto"));
                    }
                }
            }
        }

        camSettings.push_back(settings);
    }

    static recursive_mutex settingsMutex;

    void LoadSettingsFromFile(const std::string& filePath)
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
                std::ifstream file(filePath);
                
                if (file.is_open()) {
                    nlohmann::json camSettings  = nlohmann::json::parse(file);
                    file.close();

                    if (!camSettings.empty()) {
                        SetDeviceSettings(enumMoniker, camSettings);
                    }
                } else {
                    SaveSettingsToFile(filePath);
                }
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
                nlohmann::json camSettings = nlohmann::json::array();
                GetDeviceSettings(enumMoniker, camSettings);

                std::ofstream file(filePath);
                if (file.is_open()) {
                    file << std::setw(4) << camSettings << std::endl;
                    file.close();
                }
            }
        }
    }

    void SetVideoProcSetting(IAMVideoProcAmp *videoProcSettings, nlohmann::detail::iter_impl<nlohmann::json> &settings,
            tagVideoProcAmpProperty videoProcProperty,const std::string& settingName, const std::string& flagName) {
        auto setting = settings->find(settingName);
        auto flag = settings->find(flagName);
        if (setting != settings->end() && flag != settings->end()) {
            videoProcSettings->Set(videoProcProperty,
                    setting.value(),
                    flag.value() == "manual" ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
        }
    }

    void SetCamControlSetting(IAMCameraControl *camControlSettings, nlohmann::detail::iter_impl<nlohmann::json> &settings,
            tagCameraControlProperty camControlProperty, const std::string& settingName, const std::string& flagName) {
        auto setting = settings->find(settingName);
        auto flag = settings->find(flagName);
        if (setting != settings->end() && flag != settings->end()) {
            camControlSettings->Set(camControlProperty,
                    setting.value(),
                    flag.value() == "manual" ? CameraControl_Flags_Manual : CameraControl_Flags_Auto);
        }
    }
};
