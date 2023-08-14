#include<windows.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool running;
global_variable void* bitmapData;
global_variable BITMAPINFO bitmapInfo;
global_variable HBITMAP bitmapHandle;
global_variable HDC bitmapDeviceContext;
global_variable PAINTSTRUCT paint;

internal void
Win32ResizeDIBSection(int width, int height)
{
    if(bitmapHandle) {
        DeleteObject(bitmapHandle);
    }

    if(!bitmapDeviceContext) {
        bitmapDeviceContext = CreateCompatibleDC(NULL);
    }

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    bitmapInfo.bmiHeader.biSizeImage = 0;
    bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    bitmapInfo.bmiHeader.biClrUsed = 0;
    bitmapInfo.bmiHeader.biClrImportant = 0;

    bitmapHandle = CreateDIBSection(
        bitmapDeviceContext,
        &bitmapInfo,
        DIB_RGB_COLORS,
        &bitmapData,
        NULL,
        NULL
    );
}

internal void
Win32UpdateWindow(HDC deviceContext, int x, int y, int width, int height)
{
    StretchDIBits(
        deviceContext,
        x, y, width, height,
        x, y, width, height,
        bitmapData,
        &bitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

LRESULT CALLBACK
Win32MainWindowCallback(
    HWND windowHandle,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    LRESULT result = 0;
    switch(message) {
        case WM_SIZE:
        {
            RECT clientRect;
            GetClientRect(windowHandle,&clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            Win32ResizeDIBSection(width, height);
        } break;

        case WM_CLOSE: 
        {
            running = false;
        } break;

        case WM_DESTROY:
        {
            running = false;
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_PAINT: {
            HDC deviceContext = BeginPaint(windowHandle, &paint);
            int x = paint.rcPaint.left;
            int y = paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;
            int height = paint.rcPaint.bottom - paint.rcPaint.top;
            PatBlt(deviceContext, x, y, width, height, WHITENESS);
            Win32UpdateWindow(deviceContext, x, y, width, height);
            EndPaint(windowHandle, &paint);
        } break;

        default: {
            // OutputDebugStringA("default\n");
            result = DefWindowProc(windowHandle, message, wParam, lParam);
        } break;
    }

    return result;
}

int CALLBACK WinMain(
    HINSTANCE instance,
    HINSTANCE prevInstance,
    LPSTR     commandLine,
    int       showCode
)
{
    WNDCLASS windowClass = {};

    // TODO: Check if CS_OWNDC|CS_HREDRAW|CS_VREDRAW still matter.
    windowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    windowClass.lpfnWndProc = Win32MainWindowCallback;
    windowClass.hInstance = instance; // OR GetModuleHandle(0)
    // HICON     hIcon;
    windowClass.lpszClassName = "HandmadeHeroWindowClass";

    if(RegisterClass(&windowClass)) {
        //TODO: Logging
    }

    HWND windowHandle = CreateWindowEx(
        0,
        windowClass.lpszClassName,
        "Handmade Hero",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        instance,
        0);
    
    if(!windowHandle) {
        //TODO: Logging
    }

    running = true;
    while(running) {
        MSG message;
        BOOL msgRes = GetMessage(&message, 0, 0, 0);
        if(msgRes <= 0) {
            break;
        }
        TranslateMessage(&message); 
        DispatchMessage(&message); 
    }

    // ShowWindow(windowHandle, showCode);

	return 0;
}
