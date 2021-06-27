#include "Input.h"

Input::Input(HWND hwnd, size_t width, size_t height) : hwnd(hwnd), height(height), width(width), firstFrame(true), lockCursor(false)
{
	dxmouse.SetMode(DirectX::Mouse::MODE_ABSOLUTE);
	dxmouse.SetWindow(hwnd);
}

Input::~Input()
{

}

bool Input::GetKey(DirectX::Keyboard::Keys key) const
{
	return currentKeyboardState.IsKeyDown(key);
}

bool Input::GetKeyDown(DirectX::Keyboard::Keys key) const
{
	return currentKeyboardState.IsKeyDown(key) && !previosKeyboardState.IsKeyDown(key);
}

bool Input::GetKeyUp(DirectX::Keyboard::Keys key) const
{
	return !currentKeyboardState.IsKeyDown(key) && previosKeyboardState.IsKeyDown(key);
}

bool Input::GetRightMouseButtonDown() const
{
	return currentMouseState.rightButton && !previousMouseState.rightButton;
}

bool Input::GetLeftMouseButtonDown() const
{
	return currentMouseState.leftButton && !previousMouseState.leftButton;
}

POINTS Input::GetMousePosition() const
{
	POINTS points;
	points.x = currentMouseState.x;
	points.y = currentMouseState.y;
	return points;
}

POINTS Input::GetMouseDelta() const
{
	return GetMousePosition();
}

void Input::LockCursor(bool lockstate)
{
	this->lockCursor = lockstate;
	SetCursorPos(width / 2, height / 2);

	if (this->lockCursor)
	{
		RECT rect = {};
		rect.top = rect.left = 0;
		rect.right = width;
		rect.bottom = height;
		rect.top = rect.bottom = height / 2;
		rect.left = rect.right = width / 2;

		dxmouse.SetMode(DirectX::Mouse::MODE_RELATIVE);

		MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
		ClipCursor(&rect);
	}
	else
	{
		dxmouse.SetMode(DirectX::Mouse::MODE_ABSOLUTE);
		ClipCursor(NULL);
	}
}

void Input::HandleMessage(UINT umsg, WPARAM wParam, LPARAM lParam)
{
	DirectX::Keyboard::ProcessMessage(umsg, wParam, lParam);
	DirectX::Mouse::ProcessMessage(umsg, wParam, lParam);
}

void Input::Update()
{
	DirectX::Keyboard::State newKeyboardState = dxkeyboard.GetState();
	DirectX::Mouse::State newMouseState = dxmouse.GetState();

	if (firstFrame)
	{
		currentKeyboardState = previosKeyboardState = newKeyboardState;
		currentMouseState = previousMouseState = newMouseState;

		firstFrame = false;
	}
	else
	{
		previosKeyboardState = currentKeyboardState;
		currentKeyboardState = newKeyboardState;

		previousMouseState = currentMouseState;
		currentMouseState = newMouseState;
	}
}


