#include "main.h"

LRESULT CALLBACK window_proc(HWND w, UINT m, WPARAM w_param, LPARAM l_param)
{
	switch (m)
	{
	case WM_CREATE:
	{
		if (!create_facs()) return -3;
		if (!create_resources(w)) return -4;
	}
	return 0;
	case WM_PAINT:
		render(w);
		ValidateRect(w, nullptr);
		return 0;
	case WM_SIZE:
		resize(LOWORD(l_param), HIWORD(l_param));
		return 0;
	case WM_MOUSEWHEEL:
	{
		const float wheel = GET_WHEEL_DELTA_WPARAM(w_param);
		scroll_offset_y -= (font_factor * (wheel / WHEEL_DELTA));
		//redraw (send a WM_PAINT)
		InvalidateRect(w, nullptr, FALSE);
	}
	return 0;
	case WM_CLOSE:
		destroy_facs();
		destroy_resources();
		DestroyWindow(w);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcW(w, m, w_param, l_param);
}

int WINAPI wWinMain(HINSTANCE hinst, HINSTANCE h_prev, LPWSTR cmd, int show)
{
	const wchar_t* class_name = L"d2_class_name";
	const wchar_t* title = L"d2_scroll_textlayout";

	//1. register class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hinst;
	wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wc.lpfnWndProc = window_proc;
	wc.lpszClassName = class_name;
	if (!RegisterClassEx(&wc)) return -1;

	const auto window = CreateWindowExW(0, class_name, title, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, nullptr, nullptr, hinst, nullptr);
	if (window == nullptr)
	{
		UnregisterClassW(class_name, hinst);
		return -2;
	}

	ShowWindow(window, show);
	UpdateWindow(window);

	MSG msg = { 0 };
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return static_cast<int>(msg.wParam);
}