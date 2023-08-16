#include <windows.h>
#include <stdint.h>

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#define internal static
#define local_persist static
#define global_variable static

global_variable bool running;

global_variable void *bitmapMemory;
global_variable BITMAPINFO bitmapInfo;

global_variable int bitmapWidth;
global_variable int bitmapHeight;
global_variable int bytesPerPixel = 4;


internal void
RenderTestGradient(int blueOffset, int greenOffset)
{
    int pitch = bitmapWidth * bytesPerPixel;

    uint8 *row = (uint8*)bitmapMemory;
    for(int y = 0; y < bitmapHeight; ++y) {
        uint32 *pixel = (uint32 *)row;
        for(int x = 0; x < bitmapWidth; ++x) {
            uint8 blue = x + blueOffset;
            uint8 green = y + greenOffset;
            *pixel++ = blue | green << 8;
        }
        row += pitch; // can also do row = pixel
    }
}


internal void
Win32ResizeDIBSection(int width, int height)
{
    if(bitmapMemory) {
        VirtualFree(bitmapMemory, 0, MEM_RELEASE);
    }

    bitmapWidth = width;
    bitmapHeight = height;

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = bitmapWidth;
    bitmapInfo.bmiHeader.biHeight = -bitmapHeight;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    bitmapInfo.bmiHeader.biSizeImage = 0;
    bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    bitmapInfo.bmiHeader.biClrUsed = 0;
    bitmapInfo.bmiHeader.biClrImportant = 0;

    int bitmapMemorySize = (bitmapWidth*bitmapHeight)*bytesPerPixel;

    bitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32UpdateWindow(HDC deviceContext, RECT *clientRect, int x, int y, int width, int height)
{
    int windowWidth = clientRect->right - clientRect->left;
    int windowHeight = clientRect->bottom - clientRect->top;

    StretchDIBits(
        deviceContext,
        0, 0, windowWidth, windowHeight,
        0, 0, bitmapWidth, bitmapHeight,
        bitmapMemory,
        &bitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

LRESULT CALLBACK
Win32MainWindowCallback(
    HWND window,
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
            GetClientRect(window,&clientRect);
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
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            int x = paint.rcPaint.left;
            int y = paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;
            int height = paint.rcPaint.bottom - paint.rcPaint.top;

            RECT clientRect;
            GetClientRect(window, &clientRect);

            Win32UpdateWindow(deviceContext, &clientRect, x, y, width, height);
            EndPaint(window, &paint);
        } break;

        default: {
            // OutputDebugStringA("default\n");
            result = DefWindowProc(window, message, wParam, lParam);
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

    HWND window = CreateWindowEx(
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
    
    if(window)
    {
        int blueOffset = 0;
        int greenOffset = 0;

        running = true;
        while(running) {
            MSG message;
            while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
                if(message.message == WM_QUIT) {
                    running = false;
                }
                TranslateMessage(&message); 
                DispatchMessage(&message); 
            }

            RenderTestGradient(blueOffset, greenOffset);

            HDC deviceContext = GetDC(window);
            RECT clientRect;
            GetClientRect(window, &clientRect);
            int windowWidth = clientRect.right - clientRect.left;
            int windowHeight = clientRect.bottom - clientRect.top;
            Win32UpdateWindow(deviceContext, &clientRect, 0, 0, windowWidth, windowHeight);
            ReleaseDC(window, deviceContext);

            ++blueOffset;
            ++greenOffset;
        }
    }
    else
    {
        //TODO: Logging
    }


	return 0;
}
