#ifndef MBASE_ENTRY_H
#define MBASE_ENTRY_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/app/ev_loop.h>
#include <Windows.h>
#include <windowsx.h>
#include <iostream>

MBASE_BEGIN

LONG_PTR oldAddr = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        std::cout << "We are here!" << std::endl;
        return 1;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_COMMAND:
        std::cout << "Button push ?" << std::endl;
        return 1;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK BtnProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

    case WM_CAPTURECHANGED:
        std::cout << "Mouse is outside" << std::endl;
    
    }
    return CallWindowProc((WNDPROC)oldAddr, hwnd, uMsg, wParam, lParam);
}

class app {
public:
	app() {
		GetStartupInfo(&winInfo);
	}

	GENERIC run() {
        // Register the window class.
        const char CLASS_NAME[] = "Sample Window Class";

        WNDCLASSA wc = { };
        
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = CLASS_NAME;

        RegisterClass(&wc);

        // Create the window.

        HWND hwnd = CreateWindowExA(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            "Learn to Program Windows",    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,       // Parent window    
            NULL,       // Menu
            GetModuleHandle(NULL),  // Instance handle
            NULL        // Additional application data
        );

        ShowWindow(hwnd, SW_SHOWDEFAULT);
        
        HWND hwndButton = CreateWindowA(
            "BUTTON",  // Predefined class; Unicode assumed 
            "OK",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            10,         // x position 
            10,         // y position 
            100,        // Button width
            100,        // Button height
            hwnd,     // Parent window
            NULL,       // No menu.
            GetModuleHandle(NULL),
            NULL);      // Pointer not needed.

        
        oldAddr = SetWindowLongPtrA(hwndButton, GWLP_WNDPROC, (LONG_PTR)BtnProc);
        LARGE_INTEGER queryTime;
        MSG msg = { };
        LARGE_INTEGER performanceFrequency = {};
        QueryPerformanceFrequency(&performanceFrequency);
        U32 mFrequency = performanceFrequency.QuadPart;
        while (msg.message != WM_QUIT)
        {
            QueryPerformanceCounter(&queryTime);

            U32 startTime = queryTime.QuadPart;
            Sleep(15);


            if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            QueryPerformanceCounter(&queryTime);
            U32 endTime = queryTime.QuadPart;
            U32 elapsedTime = ((endTime - startTime) * 1000) / mFrequency;
            eventLoop.ManualRunTimers(elapsedTime);
           
        }
	}

    ev_loop* GetApplicationLoop() {
        return &eventLoop;
    }

private:
	
    mbase::wstring applicationName;
	STARTUPINFO winInfo;
	ev_loop eventLoop;
};

MBASE_END

#endif // MBASE_ENTRY_H