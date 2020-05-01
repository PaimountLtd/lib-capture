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
                        return j.is_object() and j.value("DevicePath", "") == pathStr and j.value("DeviceName", "") == nameStr;
                    });

                    hr = moniker->BindToObject(nullptr, nullptr, IID_IAMVideoProcAmp, (void **) &videoProcSettings);
                    if (SUCCEEDED(hr) && settings != camSettings.end()) {
                        auto backlightCompensation = settings->find("BacklightCompensation");
                        auto backLightCompensation_flag = settings->find("BacklightCompensation_flag");
                        if (backlightCompensation != settings->end() && backLightCompensation_flag != settings->end()) {
                            videoProcSettings->Set(VideoProcAmp_BacklightCompensation,
                                    backlightCompensation.value(),
                                    backLightCompensation_flag.value() == "manual" ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
                        }

                        auto brightness = settings->find("Brightness");
                        auto brightness_flag = settings->find("Brightness_flag");
                        if (brightness != settings->end() && brightness_flag != settings->end()) {
                            videoProcSettings->Set(VideoProcAmp_Brightness,
                                    brightness.value(),
                                    brightness_flag.value() == "manual" ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
                        }

                        auto colorEnable = settings->find("ColorEnable");
                        auto colorEnable_flag = settings->find("ColorEnable_flag");
                        if (colorEnable != settings->end() && colorEnable_flag != settings->end()) {
                            videoProcSettings->Set(VideoProcAmp_ColorEnable,
                                    colorEnable.value(),
                                    colorEnable_flag.value() == "manual" ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
                        }

                        auto contrast = settings->find("Contrast");
                        auto contrast_flag = settings->find("Contrast_flag");
                        if (contrast != settings->end() && contrast_flag != settings->end()) {
                            videoProcSettings->Set(VideoProcAmp_Contrast,
                                    contrast.value(),
                                    contrast_flag.value() == "manual" ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
                        }

                        auto gain = settings->find("Gain");
                        auto gain_flag = settings->find("Gain_flag");
                        if (gain != settings->end() && gain_flag != settings->end()) {
                            videoProcSettings->Set(VideoProcAmp_Gain,
                                    gain.value(),
                                    gain_flag.value() == "manual" ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
                        }

                        auto gamma = settings->find("Gamma");
                        auto gamma_flag = settings->find("Gamma_flag");
                        if (gamma != settings->end() && gamma_flag != settings->end()) {
                            videoProcSettings->Set(VideoProcAmp_Gamma,
                                    gamma.value(),
                                    gamma_flag.value() == "manual" ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
                        }

                        auto hue = settings->find("Hue");
                        auto hue_flag = settings->find("Hue_flag");
                        if (hue != settings->end() && hue_flag != settings->end()) {
                            videoProcSettings->Set(VideoProcAmp_Hue,
                                    hue.value(),
                                    hue_flag.value() == "manual" ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
                        }

                        auto saturation = settings->find("Saturation");
                        auto saturation_flag = settings->find("Saturation_flag");
                        if (saturation != settings->end() && saturation_flag != settings->end()) {
                            videoProcSettings->Set(VideoProcAmp_Saturation,
                                    saturation.value(),
                                    saturation_flag.value() == "manual" ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
                        }

                        auto sharpness = settings->find("Sharpness");
                        auto sharpness_flag = settings->find("Sharpness_flag");
                        if (sharpness != settings->end() && sharpness_flag != settings->end()) {
                            videoProcSettings->Set(VideoProcAmp_Sharpness,
                                    sharpness.value(),
                                    sharpness_flag.value() == "manual" ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
                        }

                        auto whiteBalance = settings->find("WhiteBalance");
                        auto whiteBalance_flag = settings->find("WhiteBalance_flag");
                        if (whiteBalance != settings->end() && whiteBalance_flag != settings->end()) {
                            videoProcSettings->Set(VideoProcAmp_WhiteBalance,
                                    whiteBalance.value(),
                                    whiteBalance_flag.value() == "manual" ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
                        }
                    }

                    hr = moniker->BindToObject(nullptr, nullptr, IID_IAMCameraControl, (void **) &camControlSettings);
                    if (SUCCEEDED(hr)) {
                        auto exposure = settings->find("Exposure");
                        auto exposure_flag = settings->find("Exposure_flag");
                        if (exposure != settings->end() && exposure_flag != settings->end()) {
                            camControlSettings->Set(CameraControl_Exposure,
                                    exposure.value(),
                                    exposure_flag.value() == "manual" ? CameraControl_Flags_Manual : CameraControl_Flags_Auto);
                        }

                        auto focus = settings->find("Focus");
                        auto focus_flag = settings->find("Focus_flag");
                        if (focus != settings->end() && focus_flag != settings->end()) {
                            camControlSettings->Set(CameraControl_Focus,
                                    focus.value(),
                                    focus_flag.value() == "manual" ? CameraControl_Flags_Manual : CameraControl_Flags_Auto);
                        }

                        auto iris = settings->find("Iris");
                        auto iris_flag = settings->find("Iris_flag");
                        if (iris != settings->end() && iris_flag != settings->end()) {
                            camControlSettings->Set(CameraControl_Iris,
                                    iris.value(),
                                    iris_flag.value() == "manual" ? CameraControl_Flags_Manual : CameraControl_Flags_Auto);
                        }

                        auto pan = settings->find("Pan");
                        auto pan_flag = settings->find("Pan_flag");
                        if (pan != settings->end() && pan_flag != settings->end()) {
                            camControlSettings->Set(CameraControl_Pan,
                                    pan.value(),
                                    pan_flag.value() == "manual" ? CameraControl_Flags_Manual : CameraControl_Flags_Auto);
                        }

                        auto roll = settings->find("Roll");
                        auto roll_flag = settings->find("Roll_flag");
                        if (roll != settings->end() && roll_flag != settings->end()) {
                            camControlSettings->Set(CameraControl_Roll,
                                    roll.value(),
                                    roll_flag.value() == "manual" ? CameraControl_Flags_Manual : CameraControl_Flags_Auto);
                        }

                        auto tilt = settings->find("Tilt");
                        auto tilt_flag = settings->find("Tilt_flag");
                        if (tilt != settings->end() && tilt_flag != settings->end()) {
                            camControlSettings->Set(CameraControl_Tilt,
                                    tilt.value(),
                                    tilt_flag.value() == "manual" ? CameraControl_Flags_Manual : CameraControl_Flags_Auto);
                        }

                        auto zoom = settings->find("Zoom");
                        auto zoom_flag = settings->find("Zoom_flag");
                        if (zoom != settings->end() && zoom_flag != settings->end()) {
                            camControlSettings->Set(CameraControl_Zoom,
                                    zoom.value(),
                                    zoom_flag.value() == "manual" ? CameraControl_Flags_Manual : CameraControl_Flags_Auto);
                        }
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
                
                if (file.good()) {
                    nlohmann::json camSettings  = nlohmann::json::parse(file);

                    if (!camSettings.empty()) {
                        SetDeviceSettings(enumMoniker, camSettings);
                    }
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
                if (file.good()) {
                    file << std::setw(4) << camSettings << std::endl;
                }
            }
        }
    }
};
