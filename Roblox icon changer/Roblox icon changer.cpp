#define NOMINMAX
#include <windows.h>
#include <shobjidl.h>
#include <propvarutil.h>
#include <propkey.h>
#include <iostream>
#include <thread>
#include <string>

HICON hBig = NULL;
HICON hSmall = NULL;
const char* targetWindow = "Roblox";
std::string currentTitle = "ROBLOX";

// GUI control IDs
#define ID_TEXTBOX  101
#define ID_BUTTON   102

HWND hTextBox = NULL;

void SetTaskbarIcon(HWND hwnd) {
    if (!hwnd) return;
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hBig);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hSmall);
    SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)hBig);
    SetClassLongPtr(hwnd, GCLP_HICONSM, (LONG_PTR)hSmall);
    EnumChildWindows(hwnd, [](HWND hwndChild, LPARAM) -> BOOL {
        SendMessage(hwndChild, WM_SETICON, ICON_BIG, (LPARAM)hBig);
        SendMessage(hwndChild, WM_SETICON, ICON_SMALL, (LPARAM)hSmall);
        SetClassLongPtr(hwndChild, GCLP_HICON, (LONG_PTR)hBig);
        SetClassLongPtr(hwndChild, GCLP_HICONSM, (LONG_PTR)hSmall);
        return TRUE;
        }, 0);
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME);
    IPropertyStore* pPropStore;
    HRESULT hr = SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(&pPropStore));
    if (SUCCEEDED(hr)) {
        PROPVARIANT pv;
        hr = InitPropVariantFromString(L"MyRobloxApp", &pv);
        if (SUCCEEDED(hr)) {
            pPropStore->SetValue(PKEY_AppUserModel_ID, pv);
            pPropStore->Commit();
            PropVariantClear(&pv);
        }
        pPropStore->Release();
    }
}

void IconUpdater() {
    while (true) {
        HWND hwnd = FindWindowA(NULL, targetWindow);
        if (!hwnd) hwnd = FindWindowA(NULL, currentTitle.c_str()); // also search by current title
        if (hwnd) {
            SetTaskbarIcon(hwnd);
            SetWindowTextA(hwnd, currentTitle.c_str()); // uses live title
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// GUI Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_CREATE: {
        // Label
        CreateWindowA("STATIC", "Roblox Window Title:",
            WS_VISIBLE | WS_CHILD,
            10, 10, 200, 20,
            hwnd, NULL, NULL, NULL);

        // Textbox
        hTextBox = CreateWindowA("EDIT", "ROBLOX",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            10, 35, 260, 25,
            hwnd, (HMENU)ID_TEXTBOX, NULL, NULL);

        // Button
        CreateWindowA("BUTTON", "Apply Title",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10, 70, 260, 30,
            hwnd, (HMENU)ID_BUTTON, NULL, NULL);

        // Status label
        CreateWindowA("STATIC", "Waiting for Roblox...",
            WS_VISIBLE | WS_CHILD,
            10, 110, 260, 20,
            hwnd, (HMENU)103, NULL, NULL);
        break;
    }

    case WM_COMMAND: {
        // Apply button clicked OR Enter pressed in textbox
        if (LOWORD(wParam) == ID_BUTTON ||
            (LOWORD(wParam) == ID_TEXTBOX && HIWORD(wParam) == EN_CHANGE)) {

            if (LOWORD(wParam) == ID_BUTTON) {
                char buf[256] = {};
                GetWindowTextA(hTextBox, buf, sizeof(buf));
                if (strlen(buf) > 0) {
                    currentTitle = std::string(buf);

                    // Update status label
                    std::string status = "Title set to: " + currentTitle;
                    SetWindowTextA(GetDlgItem(hwnd, 103), status.c_str());
                }
            }
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    hBig = (HICON)LoadImageA(NULL, "myicon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    hSmall = (HICON)LoadImageA(NULL, "myicon.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

    if (!hBig || !hSmall) {
        MessageBoxA(NULL, "Failed to load myicon.ico!", "Error", MB_ICONERROR);
        return 0;
    }

    // Register window class
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "RobloxIconChanger";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    // Create window
    HWND hwnd = CreateWindowA(
        "RobloxIconChanger", "Roblox Icon Changer",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        300, 175,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Start icon/title updater thread
    std::thread updater(IconUpdater);
    updater.detach();

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}