#pragma once
#include <string>
#include <queue>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <assert.h>
#include "Logger.h"

#include <Keyboard.h>
#include <Mouse.h>

class Input
{
public:
	Input(HWND, size_t width, size_t height);
	virtual ~Input();

	bool GetKey(DirectX::Keyboard::Keys key) const;
	bool GetKeyDown(DirectX::Keyboard::Keys key) const;
	bool GetKeyUp(DirectX::Keyboard::Keys key) const;

	bool GetRightMouseButtonDown() const;
	bool GetLeftMouseButtonDown() const;

	POINTS GetMousePosition() const;
	POINTS GetMouseDelta() const;

	void LockCursor(bool lockstate);
	bool IsCursorLocked() const { return this->lockCursor; }

	void HandleMessage (UINT umsg, WPARAM wParam, LPARAM lParam);
	void Update();

private:
	size_t height, width;
	HWND hwnd;
	bool lockCursor;
	bool firstFrame = true;

	DirectX::Keyboard dxkeyboard;
	DirectX::Keyboard::State currentKeyboardState;
	DirectX::Keyboard::State previosKeyboardState;

	DirectX::Mouse dxmouse;
	DirectX::Mouse::State previousMouseState;
	DirectX::Mouse::State currentMouseState;
};