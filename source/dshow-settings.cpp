#include <mutex>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <comdef.h>
#include "dshow-settings.hpp"

namespace DShow {
    void SetDeviceSettings(IEnumMoniker *enumMoniker, json_t *devices) {
        ComPtr<IMoniker> moniker;
        ComPtr<IAMVideoProcAmp> videoProcSettings;
        ComPtr<IAMCameraControl> camControlSettings;
        ComPtr<IPropertyBag> propertyBag;
        HRESULT hr;

        if (enumMoniker == nullptr) {
            return;
        }

        if (!json_is_array(devices)) {
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

                size_t index = 0;
                json_t *device = nullptr;

                hr = propertyBag->Read(L"DevicePath", &path, nullptr);
                if (SUCCEEDED(hr)) {
                    const std::string pathStr(_bstr_t(path.bstrVal, true));

                    json_array_foreach (devices, index, device) {
                        json_t * obj = json_object_get(device, "DevicePath");
                        if (!json_is_string(obj))
                            continue;

                        const char *path = json_string_value(obj);
                        if (pathStr.compare(path) == 0) {
                            device = json_array_get(devices, index);
                            break;
                        }
                    }

                    if (!device) {
                        devices = json_array();
                        json_array_append_new(devices, device);
                    }

                    if (SUCCEEDED(hr)) {
                        hr = moniker->BindToObject(nullptr, nullptr, IID_IAMVideoProcAmp, (void **) &videoProcSettings);
                        if (SUCCEEDED(hr)) {
                            SetVideoProcSetting(videoProcSettings, device, VideoProcAmp_BacklightCompensation, "BacklightCompensation", "BacklightCompensation_flag");
                            SetVideoProcSetting(videoProcSettings, device, VideoProcAmp_Brightness, "Brightness", "Brightness_flag");
                            SetVideoProcSetting(videoProcSettings, device, VideoProcAmp_ColorEnable, "ColorEnable", "ColorEnable_flag");
                            SetVideoProcSetting(videoProcSettings, device, VideoProcAmp_Contrast, "Contrast", "Contrast_flag");
                            SetVideoProcSetting(videoProcSettings, device, VideoProcAmp_Gain, "Gain", "Gain_flag");
                            SetVideoProcSetting(videoProcSettings, device, VideoProcAmp_Gamma, "Gamma", "Gamma_flag");
                            SetVideoProcSetting(videoProcSettings, device, VideoProcAmp_Hue, "Hue", "Hue_flag");
                            SetVideoProcSetting(videoProcSettings, device, VideoProcAmp_Saturation, "Saturation", "Saturation_flag");
                            SetVideoProcSetting(videoProcSettings, device, VideoProcAmp_Sharpness, "Sharpness", "Sharpness_flag");
                            SetVideoProcSetting(videoProcSettings, device, VideoProcAmp_WhiteBalance, "WhiteBalance", "WhiteBalance_flag");
                        }

                        hr = moniker->BindToObject(nullptr, nullptr, IID_IAMCameraControl, (void **) &camControlSettings);
                        if (SUCCEEDED(hr)) {
                            SetCamControlSetting(camControlSettings, device, CameraControl_Exposure, "Exposure", "Exposure_flag");
                            SetCamControlSetting(camControlSettings, device, CameraControl_Focus, "Focus", "Focus_flag");
                            SetCamControlSetting(camControlSettings, device, CameraControl_Iris, "Iris", "Iris_flag");
                            SetCamControlSetting(camControlSettings, device, CameraControl_Pan, "Pan", "Pan_flag");
                            SetCamControlSetting(camControlSettings, device, CameraControl_Roll, "Roll", "Roll_flag");
                            SetCamControlSetting(camControlSettings, device, CameraControl_Tilt, "Tilt", "Tilt_flag");
                            SetCamControlSetting(camControlSettings, device, CameraControl_Zoom, "Zoom", "Zoom_flag");
                        }
                    }
                }
            }
        }
    }

    void GetDeviceSettings(IEnumMoniker *enumMoniker, json_t *devices)
    {
        ComPtr<IMoniker> moniker;
        ComPtr<IAMVideoProcAmp> camVideoSettings;
        ComPtr<IAMCameraControl> camControlSettings;
        ComPtr<IPropertyBag> propertyBag;
        long value, flag;
        HRESULT hr;

        if (enumMoniker == nullptr) {
            return;
        }

        while (enumMoniker->Next(1, &moniker, nullptr) == S_OK) {
            hr = moniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag, (void **)&propertyBag);

            if (SUCCEEDED(hr)) {
                json_t *device = json_object();
                VARIANT path, name;
                path.vt = VT_BSTR;
                name.vt = VT_BSTR;

                hr = propertyBag->Read(L"DevicePath", &path, nullptr);
                if (SUCCEEDED(hr)) {
                    const std::string pathStr(_bstr_t(path.bstrVal, true));
                    json_object_set(device, "DevicePath", json_string(pathStr.c_str()));
                }

                hr = propertyBag->Read(L"FriendlyName", &name, nullptr);
                if (SUCCEEDED(hr)) {
                    const std::string nameStr(_bstr_t(name.bstrVal, true));
                    json_object_set(device, "DeviceName", json_string(nameStr.c_str()));
                }

                // Getting VideoProcAmp settings
                hr = moniker->BindToObject(nullptr, nullptr, IID_IAMVideoProcAmp, (void**)&camVideoSettings);
                if (SUCCEEDED(hr)) {
                    hr = camVideoSettings->Get(VideoProcAmp_BacklightCompensation, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "BacklightCompensation", json_integer(value));
                        json_object_set(device, "BacklightCompensation_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Brightness, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Brightness", json_integer(value));
                        json_object_set(device, "Brightness_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_ColorEnable, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "ColorEnable", json_integer(value));
                        json_object_set(device, "ColorEnable_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Contrast, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Contrast", json_integer(value));
                        json_object_set(device, "Contrast_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Gain, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Gain", json_integer(value));
                        json_object_set(device, "Gain_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Gamma, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Gamma", json_integer(value));
                        json_object_set(device, "Gamma_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Hue, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Hue", json_integer(value));
                        json_object_set(device, "Hue_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Saturation, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Saturation", json_integer(value));
                        json_object_set(device, "Saturation_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_Sharpness, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Sharpness", json_integer(value));
                        json_object_set(device, "Sharpness_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camVideoSettings->Get(VideoProcAmp_WhiteBalance, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "WhiteBalance", json_integer(value));

                        // Some devices return flag value 3 when the flag is auto
                        if (flag == 3) {
                            json_object_set(device, "WhiteBalance_flag", json_string("auto"));
                        } else {
                            json_object_set(device, "WhiteBalance_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                        }
                    }
                }

                // Getting CameraControl settings
                hr = moniker->BindToObject(nullptr, nullptr, IID_IAMCameraControl, (void**)&camControlSettings);
                if (SUCCEEDED(hr)) {
                    hr = camControlSettings->Get(CameraControl_Exposure, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Exposure", json_integer(value));
                        json_object_set(device, "Exposure_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camControlSettings->Get(CameraControl_Focus, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Focus", json_integer(value));
                        json_object_set(device, "Focus_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camControlSettings->Get(CameraControl_Iris, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Iris", json_integer(value));
                        json_object_set(device, "Iris_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camControlSettings->Get(CameraControl_Pan, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Pan", json_integer(value));
                        json_object_set(device, "Pan_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camControlSettings->Get(CameraControl_Roll, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Roll", json_integer(value));
                        json_object_set(device, "Roll_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camControlSettings->Get(CameraControl_Tilt, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Tilt", json_integer(value));
                        json_object_set(device, "Tilt_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }

                    hr = camControlSettings->Get(CameraControl_Zoom, &value, &flag);
                    if (SUCCEEDED(hr)) {
                        json_object_set(device, "Zoom", json_integer(value));
                        json_object_set(device, "Zoom_flag", json_string(flag > VideoProcAmp_Flags_Auto ? "manual" : "auto"));
                    }
                }

                json_array_append(devices, device);
                json_decref(device);
            }
        }
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
                json_t *devices;
                json_error_t error;

                devices = json_load_file(filePath.c_str(), 0, &error);
                if(devices) {
                    SetDeviceSettings(enumMoniker, devices);
                    json_decref(devices);
                } else
                    SaveSettingsToFile(filePath);
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
                json_t *devices = json_array();
                GetDeviceSettings(enumMoniker, devices);
                json_dump_file(devices, filePath.c_str(), JSON_INDENT(2)|JSON_ENSURE_ASCII);
                json_decref(devices);
            }
        }
    }

    void SetVideoProcSetting(IAMVideoProcAmp *videoProcSettings, json_t *device,
            tagVideoProcAmpProperty videoProcProperty,const std::string& settingName, const std::string& flagName) {
        if (!json_is_object(device))
            return;

        json_t *valueObj = json_object_get(device, settingName.c_str());

        if (json_is_integer(valueObj)) {
            int val = json_integer_value(valueObj);

            json_t *flagObbj = json_object_get(device, flagName.c_str());
            const char *flag = json_string_value(flagObbj);

            videoProcSettings->Set(videoProcProperty,
                    val,
                    strcmp(flag, "manual") == 0 ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
        }
    }

    void SetCamControlSetting(IAMCameraControl *camControlSettings, json_t *device,
            tagCameraControlProperty camControlProperty, const std::string& settingName, const std::string& flagName) {
        if (!json_is_object(device))
            return;

        json_t *valueObj = json_object_get(device, settingName.c_str());

        if (json_is_integer(valueObj)) {
            int val = json_integer_value(valueObj);

            json_t *flagObbj = json_object_get(device, flagName.c_str());
            const char *flag = json_string_value(flagObbj);

            camControlSettings->Set(camControlProperty,
                    val,
                    strcmp(flag, "manual") == 0 ? VideoProcAmp_Flags_Manual : VideoProcAmp_Flags_Auto);
        }
    }
};
