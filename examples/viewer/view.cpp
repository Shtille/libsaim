#include "model.h"
#include "controller.h"

#include <Windows.h>
#include <Windowsx.h> // for GET_X_LPARAM

#ifndef WM_MOUSEHWHEEL // It isn't defined on MinGW platform
#define WM_MOUSEHWHEEL 0x020E
#endif

namespace {
	const char * kApplicationClassName = "libsaim viewer";
	const char * kApplicationTitle = "viewer";
	const int kWidth = 640;
	const int kHeight = 480;

	Model * g_model = nullptr;
	Controller * g_controller = nullptr;
}

void OnPaint(HWND hWnd)
{
	const int width = kWidth;
	const int height = kHeight;

	int num_tiles = g_controller->Render();

	g_model->ConvertRgbToBgr();
	const void * bgr_data = g_model->data();

	// Draw bitmap onto window surface
	BITMAPINFO info;
	ZeroMemory(&info, sizeof(BITMAPINFO));
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = width;
	info.bmiHeader.biHeight = -height;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	HBITMAP cbm = CreateDIBitmap(hdc, &info.bmiHeader, CBM_INIT, bgr_data, &info, DIB_RGB_COLORS);
	HDC cdc = CreateCompatibleDC(hdc);
	HGDIOBJ obj = SelectObject(cdc, cbm);
	BitBlt(hdc, 0, 0, width, height, cdc, 0, 0, SRCCOPY);
	SelectObject(cdc, obj);
	DeleteDC(cdc);
	DeleteObject(cbm);
	EndPaint(hWnd, &ps);

	// Request redraw if need
	if (num_tiles != 0)
		InvalidateRect(hWnd, NULL, FALSE);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static POINT old_mouse_position = { 0 };
	static POINT mouse_position;
	static bool mouse_down = false;

	switch(uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		OnPaint(hWnd);
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_OEM_PLUS:
			g_controller->ZoomIn();
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case VK_OEM_MINUS:
			g_controller->ZoomOut();
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		if (!mouse_down)
		{
			mouse_down = true;
			old_mouse_position.x = GET_X_LPARAM(lParam);
			old_mouse_position.y = GET_Y_LPARAM(lParam);
		}
		break;
	case WM_LBUTTONUP:
		mouse_down = false;
		break;
	case WM_MOUSEMOVE:
		if (wParam & MK_LBUTTON)
		{
			mouse_position.x = GET_X_LPARAM(lParam);
			mouse_position.y = GET_Y_LPARAM(lParam);
			int dx = mouse_position.x - old_mouse_position.x;
			int dy = mouse_position.y - old_mouse_position.y;
			old_mouse_position = mouse_position;
			g_controller->Move(-dx, dy);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

void ProcessCommands(HWND hWnd)
{
	//RedrawWindow(hWnd, NULL, NULL, RDW_INTERNALPAINT);
}

bool Load()
{
	g_model = new Model();
	if (!g_model->Initialize())
	{
		delete g_model;
		return false;
	}
	g_model->SetTarget(kWidth, kHeight);

	g_controller = new Controller(g_model);

	return true;
}
void Unload()
{
	g_model->Deinitialize();
	delete g_model;
	delete g_controller;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Register A Window Class
	WNDCLASSEXA window_class;
	ZeroMemory(&window_class, sizeof(WNDCLASSEXA));
	window_class.cbSize = sizeof(WNDCLASSEXA);
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = (WNDPROC)(WindowProc);
	window_class.hInstance = hInstance;
	window_class.hbrBackground = (HBRUSH)NULL;
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	window_class.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	window_class.lpszClassName = kApplicationClassName;
	if (RegisterClassExA(&window_class) == 0)
	{
		MessageBox(HWND_DESKTOP, TEXT("RegisterClassEx Failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = kWidth;
	rect.bottom = kHeight;

	DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	DWORD ex_style = WS_EX_APPWINDOW;
	style |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	style |= WS_MAXIMIZEBOX | WS_SIZEBOX;
	ex_style |= WS_EX_WINDOWEDGE;
	// Adjust Window, Account For Window Borders
	AdjustWindowRectEx(&rect, style, 0, ex_style);

	HWND hwnd;
	hwnd = CreateWindowExA(ex_style,						// Extended Style
		kApplicationClassName,													// Class Name
		kApplicationTitle,														// Window Title
		style,											// Window Style
		0, 0,																	// Window X,Y Position
		rect.right - rect.left,													// Window Width
		rect.bottom - rect.top,													// Window Height
		HWND_DESKTOP,															// Desktop Is Window's Parent
		0,																		// No Menu
		hInstance,														// Pass The Window Instance
		NULL);
	if (hwnd == NULL)
	{
		UnregisterClassA(kApplicationClassName, hInstance);
		return 2;
	}

	if (!Load())
	{
		DestroyWindow(hwnd);
		UnregisterClassA(kApplicationClassName, hInstance);
		return 3;
	}

	ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
		ProcessCommands(hwnd);
	}

	DestroyWindow(hwnd);
	UnregisterClassA(kApplicationClassName, hInstance);

	Unload();

	return msg.wParam;
}