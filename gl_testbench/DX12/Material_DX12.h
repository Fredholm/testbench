#ifndef MATERIAL_DX12_H
#define MATERIAL_DX12_H

#include <d3d12.h>
#include <D3Dcompiler.h>
#pragma comment(lib,"D3DCompiler.lib")
#include "../Material.h"
#include "Utility.h"
#include "ConstantBuffer_DX12.h"

class DX12Renderer;
class Material_DX12 : public Material
{
    friend DX12Renderer;

public:
    Material_DX12();
    virtual ~Material_DX12();

    // set shader name, DOES NOT COMPILE
    virtual void setShader(const std::string& shaderFileName, ShaderType type);

    // removes any resource linked to shader type
    virtual void removeShader(ShaderType type);

    virtual void setDiffuse(Color c);

    /*
    * Compile and link all shaders
    * Returns 0  if compilation/linking succeeded.
    * Returns -1 if compilation/linking fails.
    * Error is returned in errString
    * A Vertex and a Fragment shader MUST be defined.
    * If compileMaterial is called again, it should RE-COMPILE the shader
    * In principle, it should only be necessary to re-compile if the defines set
    * has changed.
    */
    virtual int compileMaterial(std::string& errString);

    // this constant buffer will be bound every time we bind the material
    virtual void addConstantBuffer(std::string name, unsigned int location);

    // location identifies the constant buffer in a unique way
    virtual void updateConstantBuffer(const void* data, size_t size, unsigned int location);

    // activate the material for use.
    virtual int enable();

    // disable material
    virtual void disable();

private:
    std::map<unsigned int, ConstantBuffer_DX12*> constantBuffers;
};

#endif // !MATERIAL_DX12_H
