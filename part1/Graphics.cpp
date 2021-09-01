#include "Graphics.h"

HRESULT Graphics::CreateDevice(HWND window_handle, DisplayMode display_mode)
{
	HRESULT result = S_OK;

	D3D11_CREATE_DEVICE_FLAG device_flags;

#if defined(_DEBUG)
	device_flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
	};

	DXGI_SWAP_CHAIN_DESC swap_desc;
	ZeroMemory(&swap_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swap_desc.BufferCount							= 3;
	swap_desc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_desc.BufferDesc.Width						= display_mode.client_width;
	swap_desc.BufferDesc.Height						= display_mode.cilent_height;
	swap_desc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_desc.BufferDesc.RefreshRate.Numerator		= 60;
	swap_desc.BufferDesc.RefreshRate.Denominator	= 1;
	swap_desc.SampleDesc.Count						= 1;
	swap_desc.SampleDesc.Quality					= 0;
	swap_desc.Windowed								= !display_mode.full_screen;
	swap_desc.SwapEffect							= DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swap_desc.OutputWindow							= window_handle;

	result = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, device_flags,
											feature_levels, _countof(feature_levels), D3D11_SDK_VERSION, &swap_desc,
											&swap_chain, &device, NULL, &device_context);
	if (FAILED(result))
	{
		throw std::runtime_error("Device and Swap chain failed to created");
		return result;
	}

	return result;
}


HRESULT Graphics::CreateMainTarget()
{
	HRESULT result = S_OK;

	ID3D11Texture2D* color_buffer;
	swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&color_buffer);

	D3D11_TEXTURE2D_DESC color_buffer_desc;
	color_buffer->GetDesc(&color_buffer_desc);

	result = device->CreateRenderTargetView(color_buffer, NULL, &main_target_color_view);
	if (FAILED(result))
	{
		throw std::runtime_error("Device failed to create the color buffer view");
		return result;
	}

	ID3D11Texture2D* depth_buffer;
	D3D11_TEXTURE2D_DESC depth_buffer_desc;
	ZeroMemory(&depth_buffer_desc, sizeof(D3D11_TEXTURE2D_DESC));

	depth_buffer_desc.Width					= color_buffer_desc.Width;
	depth_buffer_desc.Height				= color_buffer_desc.Height;
	depth_buffer_desc.BindFlags				= D3D11_BIND_DEPTH_STENCIL;
	depth_buffer_desc.Format				= DXGI_FORMAT_R32_TYPELESS;
	depth_buffer_desc.CPUAccessFlags		= 0;
	depth_buffer_desc.ArraySize				= 1;
	depth_buffer_desc.MipLevels				= 1;
	depth_buffer_desc.Usage					= D3D11_USAGE_DEFAULT;
	depth_buffer_desc.SampleDesc.Count		= 1;
	depth_buffer_desc.SampleDesc.Quality	= 0;
	depth_buffer_desc.MiscFlags				= 0;

	result = device->CreateTexture2D(&depth_buffer_desc, NULL, &depth_buffer);
	if (FAILED(result))
	{
		throw std::runtime_error("Device failed to create the depth buffer");
		return result;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_view_desc;
	ZeroMemory(&depth_view_desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depth_view_desc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_view_desc.Format				= DXGI_FORMAT_D32_FLOAT;
	depth_view_desc.Texture2D.MipSlice	= 0;

	result = device->CreateDepthStencilView(depth_buffer, &depth_view_desc, &main_target_depth_view);
	if (FAILED(result))
	{
		throw std::runtime_error("Device failed to create the depth buffer view");
		return result;
	}

	return result;
}

void Graphics::SetViewPort(float x, float y, float width, float height)
{
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX	= x;
	viewport.TopLeftY	= y;
	viewport.Width		= width;
	viewport.Height		= height;
	viewport.MinDepth	= 0.0f;
	viewport.MaxDepth	= 1.0f;

	device_context->RSSetViewports(1, &viewport);
}

struct Vertex
{
	float position[2];
	float color[4];
};

static std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_ELEMENT_2D_POSITION = { {"POSITION_2D", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
																			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0} };

PipelineState* pipeline_state;
ID3D11Buffer* vertex_buffer;


Graphics::Graphics(HWND window_handle, DisplayMode display_mode) : used_display_mode(display_mode)
{
	if(FAILED(CreateDevice(window_handle, display_mode)))
		return;

	if (FAILED(CreateMainTarget()))
		return;


	//Create pipeline state...

	pipeline_state = new PipelineState(INPUT_ELEMENT_2D_POSITION, L"Shaders/VS.hlsl", L"Shaders/PS.hlsl", device);

	//Upload triangle...

	Vertex vertecis_positions[3] =
	{
		{{-0.5, -0.5}, {1.0, 1.0, 1.0, 1.0}},
		{{0.5, -0.5}, {1.0, 1.0, 1.0, 1.0}},
		{{0.0, 0.8}, {1.0, 1.0, 1.0, 1.0}}
	};

	D3D11_BUFFER_DESC vertex_buffer_desc;
	ZeroMemory(&vertex_buffer, sizeof(D3D11_BUFFER_DESC));

	vertex_buffer_desc.ByteWidth				= sizeof(vertecis_positions);
	vertex_buffer_desc.Usage					= D3D11_USAGE_DEFAULT;
	vertex_buffer_desc.CPUAccessFlags			= 0;
	vertex_buffer_desc.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.MiscFlags				= 0;
	vertex_buffer_desc.StructureByteStride		= sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA upload_data;
	upload_data.pSysMem				= &vertecis_positions;
	upload_data.SysMemPitch			= sizeof(vertecis_positions);
	upload_data.SysMemSlicePitch	= sizeof(vertecis_positions);

	HRESULT result = device->CreateBuffer(&vertex_buffer_desc, &upload_data, &vertex_buffer);
	if (FAILED(result))
	{
		throw std::runtime_error("Device failed to create the vertex buffer");
		return;
	}
}

void Graphics::Render()
{
	float background[4] = { 0.4, 0.7, 0.1, 1.0 };
	device_context->OMSetRenderTargets(1, &main_target_color_view, main_target_depth_view);
	device_context->ClearRenderTargetView(main_target_color_view, background);
	device_context->ClearDepthStencilView(main_target_depth_view, D3D11_CLEAR_DEPTH, 1.0f, 0.0f);

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX	= 0;
	viewport.TopLeftY	= 0;
	viewport.Width		= used_display_mode.client_width;
	viewport.Height		= used_display_mode.client_width;
	viewport.MinDepth	= 0.0f;
	viewport.MaxDepth	= 1.0f;

	pipeline_state->UsePipeline(viewport, device_context);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device_context->Draw(3, 0);

}