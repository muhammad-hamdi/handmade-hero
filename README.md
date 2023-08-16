[Handmade Hero](https://handmadehero.org/)
==========================================


## Intro

This is my personal code and study progress of the [handmade hero](https://handmadehero.org/) project.

## Progress

### Day 001: Setting Up the Windows Build
- Setup the directory structure
- Make a command line with `shell.bat` startup script to set path and `cl`
- Use `devenv` to debug in visual studio
- `WinMain`: Entry of a Windows GUI program
- `MessageBox`: Show a message box

### Day 002: Opening a Win32 Window
- Using `WNDCLASS` struct and `RegisterClass`
- Opening a window `CreateWindow`
- Get a window handle with `GetModuleHandle`
- `WndProc` and handling messages and `DefWindowProc`
- Making a loop and getting messages with `GetMessage`, `TranslateMessage`, `DispatchMessage`
- Drawing in `WM_PAIN` with `BeginPaint`, `EndPaint`, `PatBlt`

### Day 003: Allocating a Backbuffer
- Closing window with `PostQuitMessage` and handling it with a `running` variable
- #define `global_variable`, `local_persist` and internal to `static`
- Resize Buffer on recieving `WM_SIZE`
- Using `GetClientRect` to get window dimensions
- Allocating the buffer using `CreateDIBSection` using `BITMAPINFO` struct
- Drawing the buffer using `StretchDIBits`
- Creating a Device Context using `CreateCompatibleDC`
- Using `DeleteObject` to release the buffer when reallocated

#### Tangent #1
> at the end of day 3 I wanted to use the buffer to get something on the screen so I experimented a little on my own, the following is just some tinkering and will most likely be scrapped on Day 004
- Created a `RenderPixel(int x, int y, int r, int g, int b)` function
- It uses the windows `COLORREF` definition which is a uint32
- The color value is passed using the windows `RGB()` macro
- Used it to render a simple sine function across the screen
- Set `biHeight` to negative width to get a top-down coordinate where (0,0) is at top-left corner
```C
// Definition
internal void
RenderPixel(int x, int y, int r = 255, int g = 255, int b = 255) {
    int width = paint.rcPaint.right - paint.rcPaint.left; // ignore this way of getting the width, will be changed later
    ((COLORREF*)bitmapData)[y * width + x] = RGB(r, g, b);
}

// Sine function draw, placed in Win32UpdateWindow
for (int i = 0; i < width; ++i)
{
    /*      amplitude*sin(angle in radians)    */
    int y = 100*sin(i*3.1415/180); // sin function from math.h
    y = (height/2) - y; // negative y is up in top-down coords -> (0,0) at upper-left corner

    RenderPixel(i, y);
}
```

### Day 004: Animating the Backbuffer
- Using `VirtualAlloc` to allocate the bitmap memory ourselves instead of `CreateDIBSection`
- Using `VirtualFree` to free memory allocated
- Learn about `VirtualProtect` which helps in debugging __use-after-free__ errors
- Setting `biHeight` to negative to get top-left origin (I found this already during [#Tangent #1](#tangent-1))
- Pass (0,0) for x and y in `StretchDIBits` and window dimensions in dest dimensions, bitmap dimensions in src dimensions
- Render a simple gradient. Pixel has the form `0x00RRGGBB` where a padding byte is left for memory alignment
- Use `PeekMessage` instead of `GetMessage` so it doesn't block
- Draw the gradient and pass changing offsets to animate it in the main loop
- Call `Win32UpdateWindow` in the main loop to update and draw.
  -  Handle deviceContext with `GetDC(window)` and `ReleaseDC(window, deviceContext)`
  -  `GetClientRect` to get window `RECT` and dimensions