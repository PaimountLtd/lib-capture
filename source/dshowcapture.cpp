/*
 *  Copyright (C) 2023 Lain Bailey <lain@obsproject.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

#include "../dshowcapture.hpp"
#include "dshow-base.hpp"
#include "dshow-enum.hpp"
#include "dshow-dialogbox.hpp"
#include "device.hpp"
#include "dshow-device-defs.hpp"
#include "log.hpp"

#include <vector>

namespace DShow {

Device::Device(InitGraph initialize) :
	context(new HDevice),
	videoDialog(new DeviceDialogBox),
	crossbarDialog(new DeviceDialogBox),
	audioDialog(new DeviceDialogBox)
{
	if (initialize == InitGraph::True)
		context->CreateGraph();
}

Device::~Device()
{
	delete videoDialog;
	delete crossbarDialog;
	delete audioDialog;
	delete context;
}

bool Device::Valid() const
{
	return context->initialized;
}

void Device::GetAccess()
{
	context->GetAccess();
}

void Device::ReleaseAccess() 
{
	context->ReleaseAccess();
}

bool Device::ResetGraph()
{
	context->ReleaseAccess();

	// recreating a device is a cheap and easy way to clear all the filters 
	delete context;
	context = new HDevice;

	context->GetAccess();

	return context->CreateGraph();
}

void Device::ShutdownGraph()
{
	delete context;
	context = new HDevice;
}

bool Device::SetVideoConfig(VideoConfig *config)
{
	return context->SetVideoConfig(config);
}

bool Device::SetAudioConfig(AudioConfig *config)
{
	return context->SetAudioConfig(config);
}

bool Device::ConnectFilters()
{
	return context->ConnectFilters();
}

Result Device::Start()
{
	return context->Start();
}

void Device::Stop()
{
	context->Stop();
}

bool Device::GetVideoConfig(VideoConfig &config) const
{
	if (context->videoCapture == NULL)
		return false;

	config = context->videoConfig;
	return true;
}

bool Device::GetAudioConfig(AudioConfig &config) const
{
	if (context->audioCapture == NULL)
		return false;

	config = context->audioConfig;
	return true;
}

bool Device::GetVideoDeviceId(DeviceId &id) const
{
	if (context->videoCapture == NULL)
		return false;

	id = context->videoConfig;
	return true;
}

bool Device::GetAudioDeviceId(DeviceId &id) const
{
	if (context->audioCapture == NULL)
		return false;

	id = context->audioConfig;
	return true;
}

void Device::OpenDialog(void *hwnd, DialogType type) const
{
	ComPtr<IUnknown> ptr;
	HRESULT hr;

	if (type == DialogType::ConfigVideo) {
		ptr = context->videoFilter;
	} else if (type == DialogType::ConfigCrossbar ||
		   type == DialogType::ConfigCrossbar2) {
		if (context == NULL) {
			Warning(L"Invalid crossbar context");
			return;
		}
		if (context->builder == NULL) {
			Warning(L"Invalid crossbar context builder");
			return;
		}

		hr = context->builder->FindInterface(NULL, NULL,
						     context->videoFilter,
						     IID_IAMCrossbar,
						     (void **)&ptr);
		if (FAILED(hr)) {
			WarningHR(L"Failed to find crossbar", hr);
			return;
		}

		if (ptr != NULL && type == DialogType::ConfigCrossbar2) {
			ComQIPtr<IAMCrossbar> xbar(ptr);
			ComQIPtr<IBaseFilter> filter(xbar);

			hr = context->builder->FindInterface(
				&LOOK_UPSTREAM_ONLY, NULL, filter,
				IID_IAMCrossbar, (void **)&ptr);
			if (FAILED(hr)) {
				WarningHR(L"Failed to find crossbar2", hr);
				return;
			}
		}
	} else if (type == DialogType::ConfigAudio) {
		ptr = context->audioFilter;
	}

	if (!ptr) {
		Warning(L"Could not find filter to open dialog type: %d with",
			(int)type);
		return;
	}

	if (type == DialogType::ConfigVideo) {
		videoDialog->Open(ptr);
	} else if (type == DialogType::ConfigCrossbar ||
		type == DialogType::ConfigCrossbar2) {
		crossbarDialog->Open(ptr);
	} else if (type == DialogType::ConfigAudio) {
		audioDialog->Open(ptr);
	}
}

void Device::CloseDialog()
{
	videoDialog->Close();
	crossbarDialog->Close();
	audioDialog->Close();
}

static void EnumEncodedVideo(std::vector<VideoDevice> &devices,
			     const wchar_t *deviceName,
			     const wchar_t *devicePath,
			     const EncodedDevice &info)
{
	VideoDevice device;
	VideoInfo caps;

	device.name = deviceName;
	if (devicePath)
		device.path = devicePath;
	device.audioAttached = true;
	device.separateAudioFilter = false;

	caps.minCX = caps.maxCX = info.width;
	caps.minCY = caps.maxCY = info.height;
	caps.granularityCX = caps.granularityCY = 1;
	caps.minInterval = caps.maxInterval = info.frameInterval;
	caps.format = info.videoFormat;

	device.caps.push_back(caps);
	devices.push_back(device);
}

static void EnumExceptionVideoDevice(std::vector<VideoDevice> &devices,
				     IBaseFilter *filter,
				     const wchar_t *deviceName,
				     const wchar_t *devicePath)
{
	ComPtr<IPin> pin;

	if (GetPinByName(filter, PINDIR_OUTPUT, L"656", &pin))
		EnumEncodedVideo(devices, deviceName, devicePath, HD_PVR2);

	else if (GetPinByName(filter, PINDIR_OUTPUT, L"TS Out", &pin))
		EnumEncodedVideo(devices, deviceName, devicePath, Roxio);
}

static bool EnumVideoDevice(std::vector<VideoDevice> &devices,
			    IBaseFilter *filter, const wchar_t *deviceName,
			    const wchar_t *devicePath)
{
	ComPtr<IPin> pin;
	ComPtr<IPin> audioPin;
	ComPtr<IBaseFilter> audioFilter;
	VideoDevice info;

	if (wcsstr(deviceName, L"C875") != nullptr ||
	    wcsstr(deviceName, L"Prif Streambox") != nullptr ||
	    wcsstr(deviceName, L"C835") != nullptr) {
		EnumEncodedVideo(devices, deviceName, devicePath, AV_LGP);
		return true;

	} else if (wcsstr(deviceName, L"Hauppauge HD PVR Capture") != nullptr) {
		EnumEncodedVideo(devices, deviceName, devicePath, HD_PVR1);
		return true;
	}

	if (filter == NULL) {
		EnumEncodedVideo(devices, deviceName, devicePath, AV_DEFAULT);
		return true;
	}

	bool success = GetFilterPin(filter, MEDIATYPE_Video,
				    PIN_CATEGORY_CAPTURE, PINDIR_OUTPUT, &pin);

	/* if this device has no standard capture pin, see if it's an
	 * encoded device, and get its information if so (all encoded devices
	 * are exception devices pretty much) */
	if (!success) {
		EnumExceptionVideoDevice(devices, filter, deviceName,
					 devicePath);
		return true;
	}

	if (!EnumVideoCaps(pin, info.caps))
		return true;

	info.audioAttached = GetFilterPin(filter, MEDIATYPE_Audio,
					  PIN_CATEGORY_CAPTURE, PINDIR_OUTPUT,
					  &audioPin);

	// Fallback: Find a corresponding audio filter for the same device
	if (!info.audioAttached) {
		info.separateAudioFilter =
			GetDeviceAudioFilter(devicePath, &audioFilter);
		info.audioAttached = info.separateAudioFilter;
	}

	info.name = deviceName;
	if (devicePath)
		info.path = devicePath;

	devices.push_back(info);
	return true;
}

bool Device::EnumVideoDevices(std::vector<VideoDevice> &devices, bool activate)
{
	devices.clear();
	return EnumDevices(CLSID_VideoInputDeviceCategory,
			   EnumDeviceCallback(EnumVideoDevice), &devices, activate);
}

static bool EnumAudioDevice(vector<AudioDevice> &devices, IBaseFilter *filter,
			    const wchar_t *deviceName,
			    const wchar_t *devicePath)
{
	ComPtr<IPin> pin;
	AudioDevice info;
	if (filter) {
		bool success = GetFilterPin(filter, MEDIATYPE_Audio,
					PIN_CATEGORY_CAPTURE, PINDIR_OUTPUT, &pin);
		if (!success)
			return true;

		if (!EnumAudioCaps(pin, info.caps))
			return true;
	}
	info.name = deviceName;
	if (devicePath)
		info.path = devicePath;

	devices.push_back(info);
	return true;
}

bool Device::EnumAudioDevices(vector<AudioDevice> &devices, bool activate)
{
	devices.clear();
	return EnumDevices(CLSID_AudioInputDeviceCategory,
			   EnumDeviceCallback(EnumAudioDevice), &devices, activate);
}

}; /* namespace DShow */
