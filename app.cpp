#include <windows.h>
#include <stdio.h>
#include <gl\gl.h>
#include "glext.h"
#include "wglext.h"

float GetWallClockSeconds()
{
    LARGE_INTEGER counter;  QueryPerformanceCounter(&counter);
    LARGE_INTEGER freq;     QueryPerformanceFrequency(&freq);
    return (float)counter.QuadPart / (float)freq.QuadPart;
}

// if we start by compiling a fragment shader instead we get the same issue
char *vertex_shader = "#version 110 \n void main() { gl_Position = gl_Vertex; }";

PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;


int init_opengl(HDC hdc)
{
    PIXELFORMATDESCRIPTOR pfd;
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SWAP_EXCHANGE;
    pfd.iPixelType = PFD_TYPE_RGBA,
    pfd.cColorBits = 32;

    int pixel_format = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixel_format, &pfd);
    HGLRC hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);

    glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
    // other wglGetProcAddress would be here


    // create vertex shader program
    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vertex_shader, 0);

    float before = GetWallClockSeconds();

    glCompileShader(vshader);    //   <---- problem here

    float after = GetWallClockSeconds();

    char displaybuf[256];
    sprintf(displaybuf, "seconds: %f\n", after-before);
    MessageBox(0, displaybuf, "glCompileShader", 0);

    // rest of init would go here

    return 0;
}




bool running = true;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_CLOSE) running = false;
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "timer app";
    if (!RegisterClass(&wc)) { MessageBox(0, "RegisterClass failed", 0, 0); return 1; }

    HWND hwnd = CreateWindowEx(
        0, "timer app", "title", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, 0, 0, hInstance, 0);
    if (!hwnd) { MessageBox(0, "CreateWindowEx failed", 0, 0); return 1; }


    HDC hdc = GetDC(hwnd);

    init_opengl(hdc);


    while(running)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(16);
    }
    return 0;
}
