#pragma once
#include "Enviroment.h"
#include <fstream>

enum ShaderType
{
	VERTEX_SHADER,
	PIXEL_SHADER,
	GEOMETRIC_SHADER,
	HULL_SHADER,
	DOMAIN_SHADER,
	COMPUTE_SHADER
};

typedef std::vector<D3D_SHADER_MACRO> ShaderMacros;
#define SHADER_EMPTY_MACROS {{NULL, NULL}}

bool DirectoryExist(const wchar_t* directory_);
char* Get_HLSLVersion(ShaderType shader_type_, char hlsl_major_virsion, char hlsl_minor_virsion);
ID3DBlob* SimpleCompileShader(const wchar_t* directory_, ShaderType shader_type_, ShaderMacros macros_);
