#include "Material_DX12.h"

Material_DX12::Material_DX12() : Material()
{
}

Material_DX12::~Material_DX12()
{
	for (auto constantBuffer : constantBuffers)
	{
		delete constantBuffer.second;
	}
}

void Material_DX12::setShader(const std::string & shaderFileName, ShaderType type)
{
	if (shaderFileNames.find(type) != shaderFileNames.end())
	{
		removeShader(type);
	}
	shaderFileNames[type] = shaderFileName;
}

void Material_DX12::removeShader(ShaderType type)
{
}

void Material_DX12::setDiffuse(Color c)
{
}

int Material_DX12::compileMaterial(std::string & errString)
{
	
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
}

int Material_DX12::enable()
{
	return 0;
}

void Material_DX12::disable()
{
}
