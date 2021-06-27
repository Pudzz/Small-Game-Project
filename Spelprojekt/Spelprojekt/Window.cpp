#include "Window.h"

//HInstance is a Handle to the programs executable module (the .exe file in memory)
Window::Window(HINSTANCE hInstance) : hInstance(hInstance), height(DEFAULT_SCREEN_HEIGHT), width (DEFAULT_SCREEN_WIDTH), input(nullptr)
{
	windowInstance = this;
}

Window::~Window()
{
	UnregisterClass(projectTitel, hInstance);
	DestroyWindow(hwnd);
}

void Window::Initialize()
{
	const wchar_t CLASS_NAME[] = L"CoconutClass";

	WNDCLASS wndclass;
	ZeroMemory(&wndclass, sizeof(WNDCLASS));
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WindowProc;
	wndclass.lpszClassName = CLASS_NAME;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	RegisterClass(&wndclass);

	// Resizes rect to negate the window top bar. cthulhu:" necgate ltiheen"
	RECT windowRect = { 0,0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	this->hwnd = CreateWindowEx(0, CLASS_NAME, projectTitel, WS_OVERLAPPEDWINDOW, windowRect.left, windowRect.top,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, nullptr);

	assert(hwnd);
	ShowWindow(hwnd, SW_SHOW);

	this->input = new Input(hwnd, width, height);
}

void Window::ResizeWindow(size_t width, size_t height)
{
	this->width = width;
	this->height = height;
}

LRESULT Window::WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	if (windowInstance != nullptr)
		return windowInstance->m_WindowProc(hwnd, umsg, wParam, lParam);

	return DefWindowProc(hwnd, umsg, wParam, lParam);
}

LRESULT Window::m_WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{

	switch (umsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	this->input->HandleMessage(umsg, wParam, lParam);

	return DefWindowProc(hwnd, umsg, wParam, lParam);
}
