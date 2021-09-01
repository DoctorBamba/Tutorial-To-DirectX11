#include "PipelineState.h"

struct DisplayMode
{
	unsigned short client_width;
	unsigned short cilent_height;

	bool full_screen;
	bool vsync;
};


class Graphics
{
	public:
		ID3D11Device*			device;
		ID3D11DeviceContext*	device_context;

		IDXGISwapChain*			swap_chain;

		ID3D11RenderTargetView* main_target_color_view;
		ID3D11DepthStencilView* main_target_depth_view;

		DisplayMode used_display_mode;

	public:
		
		HRESULT CreateDevice(HWND window_handle, DisplayMode display_mode);
		HRESULT CreateMainTarget();

		
		Graphics(HWND window_handle, DisplayMode display_mode);

		void SetViewPort(float x, float y, float width, float height);

		void Render();
};
