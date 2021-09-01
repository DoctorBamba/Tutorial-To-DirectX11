#pragma once
#include "Graphics.h"

class WinApplication
{
	private:
		HWND window_handle;
		Graphics* graphics;

	public:
		WinApplication(HINSTANCE app_instance, DisplayMode display_mode, const wchar_t* title);
		INT Loop();
};