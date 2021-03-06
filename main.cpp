#define _WIN32_WINNT 0x0501 // Windows XP

#include <stdio.h>
#include <windows.h>
// for notification area
#include <ShellAPI.h>
// for icon loading 
#include <Commctrl.h>
// for secure strings
#include <strsafe.h>
// for resources
#include "resource.h"

#define Left					VK_LEFT
#define Right					VK_RIGHT
#define Up					VK_UP
#define Down					VK_DOWN
// CapsLock left click
#define LeftClick				VK_CAPITAL
// ScrollLock right click
#define RightClick				VK_SCROLL
// Scroll Up = Page Up
#define ScrollUp				VK_PRIOR
// Scroll Down = Page Down
#define ScrollDown				VK_NEXT
// Take me HOME!
#define Reset					VK_HOME
// F12 to turn on and off
#define KeyboardMouseToggleKey	VK_F12

HINSTANCE g_hInstance;
HHOOK hHook;
bool keyboardMouseEnabled = false;
bool keyboardMouseJustDisabled = false;
bool leftClick = false;
bool rightClick = false;
RECT currentRect;

bool KeyUpOrDown(WPARAM wParam);
bool KeyDown(WPARAM wParam);
LONG RectangleHeight(RECT r);
LONG RectangleWidth(RECT r);
void HandleKey(int nCode, WPARAM wParam, LPARAM lParam);
void DisplayNotificationIcon();
void HideNotificationIcon();
LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowsMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHook, hInstance, 0);
	if (hHook == NULL)
	{
		return -1;
	}	
	while (GetMessage(NULL, NULL, 0, 0));
	return UnhookWindowsHookEx(hHook);
}

LRESULT CALLBACK WindowsMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	return 0;
}

void DisplayNotificationIcon() 
{
	NOTIFYICONDATA nid = {};
	nid.cbSize = sizeof(nid); // ONLY supposed to WORK FOR VISTA or later	
	nid.uFlags = NIF_ICON | NIF_TIP;
	nid.uID = 1; // operate on icon 1
	StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), TEXT("Mouse Emulation Enabled"));
	if (leftClick && rightClick) {
		nid.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON_LEFTRIGHT));
	}
	else if (leftClick) {
		nid.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON_LEFT));
	}
	else if (rightClick) {
		nid.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON_RIGHT));
	}
	else {
		nid.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON));
	}
	Shell_NotifyIcon(NIM_ADD, &nid);
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void HideNotificationIcon() 
{
	NOTIFYICONDATA nid = {};
	nid.cbSize = sizeof(nid); // ONLY supposed to WORK FOR VISTA or later		
	nid.uID = 1; // operate on icon 1
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (KeyDown(wParam))
		{
			HandleKey(nCode, wParam, lParam);
		}
		if (keyboardMouseEnabled || keyboardMouseJustDisabled)
		{
			keyboardMouseJustDisabled = false;
			// don't pass keypress to Windows in general when toggled
			return 1;
		}
	}
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

bool KeyDown(WPARAM wParam)
{
	return (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN);
}

LONG RectangleHeight(RECT r)
{
	return r.bottom - r.top;
}

LONG RectangleWidth(RECT r)
{
	return r.right - r.left;
}

void HandleKey(int nCode, WPARAM wParam, LPARAM lParam)
{
	POINT currentPoint;
	GetCursorPos(&currentPoint);
	KBDLLHOOKSTRUCT *hookStruct = (KBDLLHOOKSTRUCT *)lParam;
	DWORD keyCode = hookStruct->vkCode;

	if(keyCode == KeyboardMouseToggleKey)
	{
		keyboardMouseEnabled = !keyboardMouseEnabled;
		if (!keyboardMouseEnabled) 
		{
			keyboardMouseJustDisabled = true;
		}
	}

	if(keyboardMouseEnabled)
	{
		LONG halfHeight = RectangleHeight(currentRect) / 2;
		LONG halfWidth = RectangleWidth(currentRect) / 2;
		if (keyCode == Reset)
		{
			GetWindowRect(GetDesktopWindow(), &currentRect);
			SetCursorPos(RectangleWidth(currentRect)/2, RectangleHeight(currentRect)/2);
		}
		else if (keyCode == LeftClick)
		{
			if (leftClick)
			{
				leftClick = false;
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			}
			else
			{
				leftClick = true;
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			}
		}
		else if (keyCode == RightClick)
		{
			if (rightClick)
			{
				rightClick = false;
				mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			}
			else
			{
				rightClick = true;
				mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			}
		}
		else if (keyCode == ScrollUp) 
		{
			DWORD scroll_amount = +WHEEL_DELTA;
			mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scroll_amount, 0);
		}
		else if (keyCode == ScrollDown)
		{
			DWORD scroll_amount = -WHEEL_DELTA;
			mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scroll_amount, 0);
		}
		else if (keyCode == Up)
		{
			currentRect.bottom -= halfHeight;
			SetCursorPos(currentPoint.x,
						currentPoint.y - (RectangleHeight(currentRect)/2));
		}
		else if (keyCode == Down)
		{
			currentRect.top += halfHeight;
			SetCursorPos(currentPoint.x,
						currentPoint.y + (RectangleHeight(currentRect)/2));
		}
		else if (keyCode == Left)
		{
			currentRect.right -= halfWidth;
			SetCursorPos(currentPoint.x - (RectangleWidth(currentRect)/2),
						currentPoint.y);
		}
		else if (keyCode == Right)
		{
			currentRect.left += halfWidth;
			SetCursorPos(currentPoint.x + (RectangleWidth(currentRect)/2),
						currentPoint.y);
		}
		DisplayNotificationIcon();
	}
	else 
	{
		HideNotificationIcon();
	}
}
