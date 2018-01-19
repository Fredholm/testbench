#include "Material_DX12.h"

Material_DX12::Material_DX12(const std::string & name)
{
    isValid = false;
    color   = { 1, 1, 1, 1 };
}

Material_DX12::~Material_DX12() { }

void Material_DX12::setShader(const std::string & shaderFileName, ShaderType type)
{
    // Remove current
    if (shaderFileNames.find(type) != shaderFileNames.end())
        removeShader(type);

    // Set new shader
    shaderFileNames[type] = shaderFileName;
}

void Material_DX12::removeShader(ShaderType type)
{

}

void Material_DX12::setDiffuse(Color c)
{
    color = c;
}

int Material_DX12::compileMaterial(std::string & errString)
{
    removeShader(ShaderType::VS);
    removeShader(ShaderType::PS);

    //
    // Temp Compiling
    //

    ID3DBlob* vertexShader;
    ID3DBlob* pixelShader;

#if defined(_DEBUG)
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ThrowIfFailed(D3DCompileFromFile(L"../assets/DX12/shader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
    ThrowIfFailed(D3DCompileFromFile(L"../assets/DX12/shader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },    // 12 Bytes = 3 * 4
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }    // 16 Bytes = 4 * 4
    };

    return 0;
}

void Material_DX12::addConstantBuffer(std::string name, unsigned int location)
{
//  constantBuffers[location] = new ConstantBuffer_DX12(name, location);
}

void Material_DX12::updateConstantBuffer(const void * data, size_t size, unsigned int location)
{
//  constantBuffers[location]->setData(data, size, this, location);
}

int Material_DX12::enable()
{
    // I need the Pipeline State in here?
    return 0;
}

void Material_DX12::disable()
{
    // I need the Pipeline State in here?
}
