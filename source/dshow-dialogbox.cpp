#include "dshow-dialogbox.hpp"
#include "log.hpp"

namespace DShow {
    DeviceDialogBox::DeviceDialogBox() :
        deviceFilter(nullptr),
        threadId(0),
        threadHandle(nullptr),
        isOpen(false)
    {
    }

    void DeviceDialogBox::Open(IUnknown* filter) {
        if (!isOpen || WaitForSingleObject(threadHandle, 0) == WAIT_OBJECT_0) {
            deviceFilter = filter;
            threadHandle = CreateThread(nullptr, 0, CallCreate, (void*) this, 0, &threadId);

            if (threadHandle == nullptr) {
                Warning(L"Could not create thread for filter dialog box");
            }
        }
    }

    void DeviceDialogBox::Close() {
        if (isOpen) {
            PostThreadMessage(threadId, WM_QUIT, 0, 0);
            CloseHandle(threadHandle);
            isOpen = false;
        }
    }

    DWORD DeviceDialogBox::Create() {
        ComQIPtr<ISpecifyPropertyPages> pages(deviceFilter);
        CAUUID cauuid;

        if (pages != nullptr) {
            if (SUCCEEDED(pages->GetPages(&cauuid)) && cauuid.cElems) {
                isOpen = true;
                OleCreatePropertyFrame(nullptr, 0, 0, nullptr, 1,
                                       (LPUNKNOWN *)&deviceFilter,
                                       cauuid.cElems, cauuid.pElems, 0,
                                       0, nullptr);
                CoTaskMemFree(cauuid.pElems);
            }
        }

        return 0;
    }
}
