#include <windows.h>
#include <stdint.h>
#include <winerror.h>
#include <xinput.h>

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

struct win32_window_dimensions {
    int width;
    int height;
};

struct win32_offscreen_buffer {
    // NOTE: Pixels are always 32-bits wide, Little Endian 0x XX RR GG BB, Memory Order BB GG RR XX
    BITMAPINFO info;
    void *memory;
    int width;
    int height;
    int pitch;
};

global_variable bool running;
global_variable win32_offscreen_buffer mainBackBuffer;

// NOTE: XInputGetState
#define XINPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef XINPUT_GET_STATE(x_input_get_state);
XINPUT_GET_STATE(XInputGetStateStub)
{
    return(0);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

// NOTE: XInputSetState
#define XINPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef XINPUT_SET_STATE(x_input_set_state);
XINPUT_SET_STATE(XInputSetStateStub)
{
    return(0);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void
Win32LoadXInput(void)
{
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if(XInputLibrary)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
    }
}

internal void
RenderTestGradient(win32_offscreen_buffer *buffer, int blueOffset, int greenOffset)
{

    uint8 *row = (uint8*)buffer->memory;
    for(int y = 0; y < buffer->height; ++y) {
        uint32 *pixel = (uint32 *)row;
        for(int x = 0; x < buffer->width; ++x) {
            uint8 blue = x + blueOffset;
            uint8 green = y + greenOffset;
            *pixel++ = blue | green << 8;
        }
        row += buffer->pitch; // can also do row = pixel
    }
}

internal win32_window_dimensions
Win32GetWindowDimensions(HWND window)
{
    RECT clientRect;
    GetClientRect(window, &clientRect);
    int windowWidth = clientRect.right - clientRect.left;
    int windowHeight = clientRect.bottom - clientRect.top;

    return {windowWidth, windowHeight};
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *buffer, int width, int height)
{
    if(buffer->memory) {
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    buffer->width = width;
    buffer->height = height;
    int bytesPerPixel = 4;

    buffer->pitch = buffer->width * bytesPerPixel;

    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biWidth = buffer->width;
    buffer->info.bmiHeader.biHeight = -buffer->height;
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32;
    buffer->info.bmiHeader.biCompression = BI_RGB;
    buffer->info.bmiHeader.biSizeImage = 0;
    buffer->info.bmiHeader.biXPelsPerMeter = 0;
    buffer->info.bmiHeader.biYPelsPerMeter = 0;
    buffer->info.bmiHeader.biClrUsed = 0;
    buffer->info.bmiHeader.biClrImportant = 0;

    int bitmapMemorySize = (buffer->width*buffer->height)*bytesPerPixel;

    buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32DisplayBufferInWindow(win32_offscreen_buffer *buffer,
                        HDC deviceContext,
                        int windowWidth, int windowHeight)
{
    StretchDIBits(
        deviceContext,
        0, 0, windowWidth, windowHeight,
        0, 0, buffer->width, buffer->height,
        buffer->memory,
        &buffer->info,
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
        case WM_CLOSE:
        {
            running = false;
        } break;

        case WM_DESTROY:
        {
            running = false;
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32 VKCode = wParam;
            bool WasDown = (lParam & (1 << 30)) != 0; // NOTE: check msdn for lParam additional info in key messages
            bool IsDown = (lParam & (1 << 31)) == 0;

            if(VKCode == 'W')
            {
            }
            else if(VKCode == 'A')
            {
            }
            else if(VKCode == 'S')
            {
            }
            else if(VKCode == 'D')
            {
            }
            else if(VKCode == 'Q')
            {
            }
            else if(VKCode == 'E')
            {
            }
            else if(VKCode == VK_UP)
            {
            }
            else if(VKCode == VK_LEFT)
            {
            }
            else if(VKCode == VK_DOWN)
            {
            }
            else if(VKCode == VK_RIGHT)
            {
            }
            else if(VKCode == VK_ESCAPE)
            {
            }
            else if(VKCode == VK_SPACE)
            {
            }
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint);
            win32_window_dimensions dimensions = Win32GetWindowDimensions(window);
            Win32DisplayBufferInWindow(&mainBackBuffer, deviceContext, dimensions.width, dimensions.height);
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
    Win32LoadXInput();

    WNDCLASSA windowClass = {};

    Win32ResizeDIBSection(&mainBackBuffer, 1280, 720);

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
        HDC deviceContext = GetDC(window);
        SetStretchBltMode(deviceContext, COLORONCOLOR); // COLORONCOLOR = STRETCH_DELETESCANS

        int XOffset = 0;
        int YOffset = 0;

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

            for (DWORD ControllerIndex = 0;
                ControllerIndex < XUSER_MAX_COUNT;
                ++ControllerIndex)
            {
                XINPUT_STATE ControllerState;
                if(XInputGetState(ControllerIndex,&ControllerState) == ERROR_SUCCESS)
                {
                    // NOTE: This controller is plugged in
                    // TODO: see if ControllerState.dwPacketNumber increments too rapidly

                    XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
                    bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                    bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                    bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                    bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                    bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
                    bool Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
                    // bool LeftThumb = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
                    // bool RightThumb = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
                    bool LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                    bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                    bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
                    bool BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
                    bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
                    bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);

                    int16 StickX = Pad->sThumbLX;
                    int16 StickY = Pad->sThumbLY;

                    if(AButton) {
                        YOffset += 2;
                    }
                }
                else
                {
                    // NOTE: The controller is not available
                }
            }

            RenderTestGradient(&mainBackBuffer, XOffset, YOffset);

            win32_window_dimensions dimensions = Win32GetWindowDimensions(window);
            Win32DisplayBufferInWindow(&mainBackBuffer, deviceContext, dimensions.width, dimensions.height);

            ++XOffset;
        }
    }
    else
    {
        //TODO: Logging
    }


	return 0;
}
