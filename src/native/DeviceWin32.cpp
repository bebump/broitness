#include "../Device.h"

#include <juce_events/juce_events.h>

#define _UNICODE 1
#define UNICODE 1
#define NOMINMAX
#include <windows.h>
#include <dbt.h>

using namespace juce;

class HiddenMessageWindow
{
public:
    HiddenMessageWindow (const TCHAR* const messageWindowName, WNDPROC wndProc)
    {
        String className ("BROITNESS_");
        className << String::toHexString (Time::getHighResolutionTicks());

        HMODULE moduleHandle = (HMODULE) Process::getCurrentModuleInstanceHandle();

        WNDCLASSEX wc = {};
        wc.cbSize         = sizeof (wc);
        wc.lpfnWndProc    = wndProc;
        wc.cbWndExtra     = 4;
        wc.hInstance      = moduleHandle;
        wc.lpszClassName  = className.toWideCharPointer();

        atom = RegisterClassEx (&wc);
        jassert (atom != 0);

        hwnd = CreateWindow (getClassNameFromAtom(), messageWindowName,
                             0, 0, 0, 0, 0,
                             nullptr, nullptr, moduleHandle, nullptr);
        jassert (hwnd != nullptr);
    }

    ~HiddenMessageWindow()
    {
        DestroyWindow (hwnd);
        UnregisterClass (getClassNameFromAtom(), nullptr);
    }

    inline HWND getHWND() const noexcept     { return hwnd; }

private:
    ATOM atom;
    HWND hwnd;

    LPCTSTR getClassNameFromAtom() noexcept  { return (LPCTSTR) (pointer_sized_uint) atom; }
};

struct HDevNotifyDeleter
{
    void operator() (HDEVNOTIFY handle) const
    {
        if (handle != nullptr)
            UnregisterDeviceNotification (handle);
    }
};

using ManagedHDevNotify = std::unique_ptr<std::remove_pointer_t<HDEVNOTIFY>, HDevNotifyDeleter>;

static GUID getGuid (DeviceChangeDetector::DeviceType type)
{
    switch (type)
    {
        case DeviceChangeDetector::DeviceType::hid:
            return { 0x4D1E55B2, 0xF16F, 0x11CF, 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 };
    }

    jassertfalse;

    return { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
}

class DeviceChangeDetector::Impl
{
public:
    Impl (DeviceChangeDetector& ownerIn, String name, DeviceType typeIn)
        : owner (ownerIn),
          messageWindow (name.toWideCharPointer(), (WNDPROC) deviceChangeEventCallback),
          type (typeIn)
    {
        SetWindowLongPtr (messageWindow.getHWND(), GWLP_USERDATA, (LONG_PTR) this);

        if (type == DeviceType::hid)
        {
            DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

            ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
            NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
            NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
            NotificationFilter.dbcc_classguid = getGuid (type);

            // DEVICE_NOTIFY_ALL_INTERFACE_CLASSES could be used to ignore the NotificationFilter
            // and register for everything.
            hDevNotify.reset (RegisterDeviceNotification (messageWindow.getHWND(),
                                                          &NotificationFilter,
                                                          DEVICE_NOTIFY_WINDOW_HANDLE));
        }
    }

private:
    static LRESULT CALLBACK deviceChangeEventCallback (HWND h, const UINT message,
                                                       const WPARAM wParam, const LPARAM lParam)
    {
        reinterpret_cast<Impl*> (GetWindowLongPtr (h, GWLP_USERDATA))->handleMessage (h,
                                                                                      message,
                                                                                      wParam,
                                                                                      lParam);

        return DefWindowProc (h, message, wParam, lParam);
    }

    void handleMessage (HWND h, const UINT message, const WPARAM wParam, const LPARAM lParam)
    {
        if (message == WM_DEVICECHANGE
            && (   wParam == DBT_DEVICEARRIVAL
                || wParam == DBT_DEVICEREMOVECOMPLETE
                || wParam == DBT_DEVNODES_CHANGED))
        {
            if (wParam == DBT_DEVICEARRIVAL || wParam == DBT_DEVICEREMOVECOMPLETE)
                if (reinterpret_cast<DEV_BROADCAST_HDR*> (lParam)->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                    if (reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE*> (lParam)->dbcc_classguid == getGuid (type))
                        owner.callback (wParam == DBT_DEVICEREMOVECOMPLETE ? DeviceEvent::removeComplete
                                                                           : DeviceEvent::arrival,
                                        type);

            return;
        }

        if (message == WM_DISPLAYCHANGE && type == DeviceType::display)
        {
            owner.callback (DeviceEvent::displayChange, type);
            return;
        }
    }

    DeviceChangeDetector& owner;
    HiddenMessageWindow messageWindow;
    DeviceType type;
    ManagedHDevNotify hDevNotify;
};

DeviceChangeDetector::DeviceChangeDetector (juce::String name,
                                            DeviceType type,
                                            std::function<void(DeviceEvent, DeviceType)> cb)
    : callback (std::move (cb)),
      impl (new Impl (*this, std::move (name), type))
{
    jassert (callback);
}

DeviceChangeDetector::~DeviceChangeDetector() = default;
