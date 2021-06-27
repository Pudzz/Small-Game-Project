#pragma once
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h> 

#include <assert.h>
#include "Input.h"

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 720

class Window
{
public:
	Window(HINSTANCE hInstance);
	virtual ~Window();

	void Initialize();
	void ResizeWindow(size_t width, size_t height);
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);

	size_t GetWidth() const { return this->width; }
	size_t GetHeight() const { return this->height; }
	HWND GetHWND() const { return this->hwnd; }
	HINSTANCE GetHInstance() const { return this->hInstance; }
	Input* GetInput() const { return this->input; }

private:
	LRESULT CALLBACK m_WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);

private:
	const wchar_t* projectTitel = L"Game of Cones - Litet spelprojekt";
	size_t width, height;
	HWND hwnd;
	HINSTANCE hInstance;
	Input* input;
};

static Window* windowInstance;