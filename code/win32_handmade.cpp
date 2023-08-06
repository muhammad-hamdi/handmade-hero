#include<windows.h>

LRESULT CALLBACK
MainWindowCallback(
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
            OutputDebugStringA("WM_SIZE\n");
        } break;

        case WM_CLOSE: 
        {
            OutputDebugStringA("WM_CLOSE\n");
        } break;

        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(windowHandle, &paint);
            int x = paint.rcPaint.left;
            int y = paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;
            int height = paint.rcPaint.bottom - paint.rcPaint.top;
            PatBlt(deviceContext, x, y, width, height, WHITENESS);
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
    windowClass.lpfnWndProc = MainWindowCallback;
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

    MSG message;
    BOOL msgRes;
    while(msgRes = GetMessage(&message, 0, 0, 0)) {
        if(msgRes <= 0) {
            break;
        }
        TranslateMessage(&message); 
        DispatchMessage(&message); 
    }

    // ShowWindow(windowHandle, showCode);

	return 0;
}
