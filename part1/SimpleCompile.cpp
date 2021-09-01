#include "SimpleCompile.h"

bool DirectoryExist(const wchar_t* directory_)
{
	std::ifstream f(directory_);
	return f.good();
}

char* Get_HLSLVersion(ShaderType shader_type_, char hlsl_major_virsion, char hlsl_minor_virsion)
{
	char t;
	switch (shader_type_)
	{
	case VERTEX_SHADER:
		t = 'v';
		break;
	case PIXEL_SHADER:
		t = 'p';
		break;
	case GEOMETRIC_SHADER:
		t = 'g';
		break;
	case HULL_SHADER:
		t = 'h';
		break;
	case DOMAIN_SHADER:
		t = 'd';
		break;
	case COMPUTE_SHADER:
		t = 'c';
		break;
	}

	char* str = new char[7];
	str[0] = t;
	str[1] = 's';
	str[2] = '_';
	str[3] = (char)(hlsl_major_virsion + 48);
	str[4] = '_';
	str[5] = (char)(hlsl_minor_virsion + 48);
	str[6] = '\0';

	return str;
}

ID3DBlob* SimpleCompileShader(const wchar_t* directory_, ShaderType shader_type_, ShaderMacros macros_)
{
	HRESULT result = S_OK;

	ID3DBlob* shader_binary;
	ID3DBlob* error_blob;

	if (directory_ == L"")
		return nullptr;

	if (!DirectoryExist(directory_))
	{
		std::wstring msg = std::wstring(L"Shader directory -") + std::wstring(directory_) + std::wstring(L" not fuond");
		OutputDebugStringW(msg.c_str());

		return nullptr;
	}

	result = D3DCompileFromFile(directory_,
								macros_.data(),
								D3D_COMPILE_STANDARD_FILE_INCLUDE,
								"main",
								Get_HLSLVersion(shader_type_, D3D11_SHADER_MAJOR_VERSION, D3D11_SHADER_MINOR_VERSION),
								0,
								0,
								&shader_binary,
								&error_blob);
	if (FAILED(result))
	{
		OutputDebugStringA((char*)error_blob->GetBufferPointer());
		return nullptr;
	}

	return shader_binary;
}