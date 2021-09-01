#pragma once
#include "Enviroment.h"
#include "SimpleCompile.h"

class PipelineState
{
	private:
		ID3D11InputLayout*		input_layout;
		ID3D11VertexShader*		vertex_shader;
		ID3D11RasterizerState*	rasterizer_state;
		ID3D11PixelShader*		pixel_shader;


	public:

		HRESULT CreateStandartRasterizerState(ID3D11Device* device)
		{
			HRESULT result = S_OK;

			D3D11_RASTERIZER_DESC rast_desc;
			ZeroMemory(&rast_desc, sizeof(D3D11_RASTERIZER_DESC));
			rast_desc.FillMode				= D3D11_FILL_SOLID;
			rast_desc.CullMode				= D3D11_CULL_FRONT;
			rast_desc.FrontCounterClockwise = false;
			rast_desc.DepthBias				= 0.0f;
			rast_desc.DepthBiasClamp		= 0.0f;
			rast_desc.SlopeScaledDepthBias	= 0.0f;
			rast_desc.ScissorEnable			= false;
			rast_desc.AntialiasedLineEnable = false;

			result = device->CreateRasterizerState(&rast_desc, &rasterizer_state);
			if (FAILED(result))
			{
				throw std::runtime_error("Device failed to create rasterizer state");
				return result;
			}

			return result;
		}


		PipelineState(std::vector<D3D11_INPUT_ELEMENT_DESC> inputs, std::wstring vertex_shader_directory, std::wstring pixel_shader_directory, ID3D11Device* device)
		{
			HRESULT result = S_OK;
			if(FAILED(CreateStandartRasterizerState(device)))
				return;


			ID3DBlob* vertex_shader_binary = SimpleCompileShader(vertex_shader_directory.c_str(), VERTEX_SHADER, SHADER_EMPTY_MACROS);
			if (vertex_shader_binary == nullptr)
			{
				throw std::runtime_error("Vertex Shader Complie Error - Please look at the Debuger's output");
				return;
			}

			ID3DBlob* pixel_shader_binary  = SimpleCompileShader(pixel_shader_directory.c_str(), PIXEL_SHADER, SHADER_EMPTY_MACROS);
			if (pixel_shader_binary == nullptr)
			{
				throw std::runtime_error("Pixel Shader Complie Error - Please look at the Debuger's output");
				return;
			}

			
			result = device->CreateVertexShader(vertex_shader_binary->GetBufferPointer(), vertex_shader_binary->GetBufferSize(), nullptr, &vertex_shader);
			if(FAILED(result))
			{
				throw std::runtime_error("Device failed to create vertex shader from bytecode");
				return;
			}

			result = device->CreatePixelShader(pixel_shader_binary->GetBufferPointer(), pixel_shader_binary->GetBufferSize(), nullptr, &pixel_shader);
			if (FAILED(result))
			{
				throw std::runtime_error("Device failed to create pixel shader from bytecode");
				return;
			}


			result == device->CreateInputLayout(inputs.data(), inputs.size(), vertex_shader_binary->GetBufferPointer(), vertex_shader_binary->GetBufferSize(), &input_layout);
			if (FAILED(result))
			{
				throw std::runtime_error("Device failed to create input layout for this pipeline");
				return;
			}
		}

		void UsePipeline(D3D11_VIEWPORT view_port, ID3D11DeviceContext* device_context)
		{
			device_context->IASetInputLayout(input_layout);
			device_context->VSSetShader(vertex_shader, nullptr, 0);
			device_context->RSSetState(rasterizer_state);
			device_context->RSSetViewports(1, &view_port);
			device_context->PSSetShader(pixel_shader, nullptr, 0);
		}


};