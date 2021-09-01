#include "WinApplication.h"

LRESULT WINAPI WinProc(HWND wnd, UINT msg_code, WPARAM wparam, LPARAM lparam)
{
	switch (msg_code)
	{
		case WM_KEYDOWN:
		{
			if ((UINT)wparam == VK_ESCAPE)
				PostMessageW(wnd, WM_DESTROY, 0, 0);

			return 0;
			break;
		}

		case WM_DESTROY:
		{
			ExitProcess(0);
			return 0;

			break;
		}

	default:
		break;
	}

	return DefWindowProc(wnd, msg_code, wparam, lparam);
}


WinApplication::WinApplication(HINSTANCE app_instance, DisplayMode display_mode, const wchar_t* title)
{
	WNDCLASSEX window_class = {};
	ZeroMemory(&window_class, sizeof(WNDCLASSEX));

	window_class.cbSize			= sizeof(WNDCLASSEX);
	window_class.hbrBackground	= (HBRUSH)COLOR_WINDOW;
	window_class.lpszClassName	= L"WindowClass";
	window_class.hCursor		= LoadCursor(NULL, IDC_ARROW);
	window_class.style			= CS_HREDRAW | CS_VREDRAW;
	window_class.hInstance		= app_instance;
	window_class.lpfnWndProc	= WinProc;

	RegisterClassExW(&window_class);

	RECT window_rect = { 0, 0, display_mode.client_width, display_mode.cilent_height };
	AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);

	window_handle = CreateWindowExW(NULL, L"WindowClass", title, WS_OVERLAPPEDWINDOW, 100, 100,
									window_rect.right - window_rect.left,
									window_rect.bottom - window_rect.top, NULL, FALSE, app_instance, NULL);

	ShowWindow(window_handle, 1);

	//Install Graphics...

	try
	{
		graphics = new Graphics(window_handle, display_mode);
	}
	catch(std::exception& error)
	{
		std::string error_msg(error.what());
		std::wstring error_wmsg = std::wstring(error_msg.begin(), error_msg.end());

		MessageBox(window_handle, error_wmsg.c_str(), L"Graphics Installation Error!!!", MB_OK);
	}
}

INT WinApplication::Loop()
{
	MSG msg = {};

	while (msg.message != WM_QUIT)
	{
		while (PeekMessage(&msg, window_handle, 0, 0, 1))
		{
			TranslateMessage(&msg);

			DispatchMessageW(&msg);
		}

		graphics->Render();

		graphics->swap_chain->Present((graphics->used_display_mode.vsync) ? 1 : 0, 0);
	}

	return 0;
}