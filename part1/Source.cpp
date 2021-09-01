#include "WinApplication.h"

INT WinMain(HINSTANCE app_instance, HINSTANCE previous_instance, LPSTR lp_cmd, INT show_cmd)
{
	DisplayMode display_mode;
	display_mode.client_width	= 500;
	display_mode.cilent_height	= 500;
	display_mode.full_screen	= false;
	display_mode.vsync			= true;

	WinApplication* Application = new WinApplication(app_instance, display_mode, L"Window");

	return Application->Loop();
}