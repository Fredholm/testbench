#include "Material_DX12.h"
#include <fstream>
#include <vector>

#include "../IA.h"

Material_DX12::Material_DX12() : Material()
{
}

Material_DX12::~Material_DX12()
{
	for (auto constantBuffer : constantBuffers)
	{
		delete constantBuffer.second;
	}

	for (auto blob : blobs)
	{
		blob.second->Release();
	}
}

void Material_DX12::setShader(const std::string & shaderFileName, ShaderType type)
{
	removeShader(type);
	shaderFileNames[type] = shaderFileName;
}

void Material_DX12::removeShader(ShaderType type)
{
	auto it1 = shaderFileNames.find(type);
	if (it1 != shaderFileNames.end())
	{
		shaderFileNames.erase(it1);
	}

	auto it2 = blobs.find(type);
	if (it2 != blobs.end())
	{
		it2->second->Release();
		blobs.erase(it2);
	}
}

void Material_DX12::setDiffuse(Color c)
{
}

int Material_DX12::compileMaterial(std::string & errString)
{
	static std::map<ShaderType, std::string> entryPoints = { 
		{ShaderType::VS, "VSMain"},
		{ShaderType::PS, "PSMain"},
	};
	static std::map<ShaderType, std::string> shaderModels = {
		{ ShaderType::VS, "vs_5_1" },
		{ ShaderType::PS, "ps_5_1" },
	};

	for (auto pair : shaderFileNames)
	{
		ShaderType shaderType = pair.first;
		std::string fileName = pair.second;

		ID3DBlob * blob, * error;

		std::ifstream infile(fileName);
		std::string fileContent((std::istreambuf_iterator<char>(infile)),
						 std::istreambuf_iterator<char>());

		std::string defines;
		for (auto define : shaderDefines[shaderType])
		{
			defines += define.rem;
		}

		std::string shaderCode = defines + fileContent;

		UINT compileFlags = 0;
#ifdef _DEBUG
		compileFlags |= D3DCOMPILE_DEBUG;
		compileFlags |= D3DCOMPILE_SKIP_VALIDATION;
#endif

		auto hr = D3DCompile(
			shaderCode.c_str(),
			shaderCode.length(),
			fileName.c_str(),
			nullptr,
			nullptr,
			entryPoints[shaderType].c_str(),
			shaderModels[shaderType].c_str(),
			compileFlags,
			0,
			&blob,
			&error
		);

		if FAILED(hr)
		{
			errString = reinterpret_cast<char*>(error->GetBufferPointer());
			return -1;
		}
	}
	
	return 0;
}

void Material_DX12::addConstantBuffer(std::string name, unsigned int location)
{
	if (constantBuffers.find(location) != constantBuffers.end())
	{
		delete constantBuffers[location];
	}

	constantBuffers[location] = new ConstantBuffer_DX12(location);
}

void Material_DX12::updateConstantBuffer(const void * data, size_t size, unsigned int location)
{
	constantBuffers[location]->setData(data, size);
}

int Material_DX12::enable()
{
	return 0;
}

void Material_DX12::disable()
{
}
