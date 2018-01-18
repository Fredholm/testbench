#ifndef MATERIAL_DX12_H
#define MATERIAL_DX12_H

#include <d3d12.h>
#include "Material.h"

class DX12Renderer;
class Material_DX12 : public Material
{
    friend DX12Renderer;
public:
    Material_DX12(const std::string& name);
    ~Material_DX12();

    void setShader(const std::string& shaderFileName, ShaderType type);
    void removeShader(ShaderType type);
    int compileMaterial(std::string& errString);
    int enable();
    void disable();
    UINT getProgram() { return program; };
    void setDiffuse(Color c);

    // location identifies the constant buffer in a unique way
    void updateConstantBuffer(const void* data, size_t size, unsigned int location);
    // slower version using a string
    void addConstantBuffer(std::string name, unsigned int location);
    std::map<unsigned int, ConstantBufferGL*> constantBuffers;
};

#endif // !MATERIAL_DX12_H
