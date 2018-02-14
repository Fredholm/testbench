#ifndef MATERIAL_DX12_H
#define MATERIAL_DX12_H

#include <d3d12.h>
#include <D3Dcompiler.h>
#pragma comment(lib,"D3DCompiler.lib")
#include "../Material.h"
#include "Utility.h"
#include "ConstantBuffer_DX12.h"

class Material_DX12 : public Material
{
public:
	Material_DX12();
	virtual ~Material_DX12();

	// Inherited via Material
	virtual void setShader(const std::string & shaderFileName, ShaderType type) override;

	virtual void removeShader(ShaderType type) override;

	virtual void setDiffuse(Color c) override;

	virtual int compileMaterial(std::string & errString) override;

	virtual void addConstantBuffer(std::string name, unsigned int location) override;

	virtual void updateConstantBuffer(const void * data, size_t size, unsigned int location) override;

	virtual int enable() override;

	virtual void disable() override;

private:
	std::map<UINT, ConstantBuffer_DX12*> constantBuffers;
};

#endif // !MATERIAL_DX12_H
