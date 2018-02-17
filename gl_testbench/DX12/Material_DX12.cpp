#include "Material_DX12.h"
#include "Helper.h"
#include <d3dcompiler.h>
#include <fstream>

#pragma comment(lib, "d3dcompiler.lib")

Material_DX12::Material_DX12(const std::string& name)
{
}

Material_DX12::~Material_DX12()
{
    for (auto & type_fileName : shaderFileNames)
    {
        removeShader(type_fileName.first);
    }

    for (auto & location_constantBuffer : constantBuffers)
    {
        delete location_constantBuffer.second;
    }
}

void Material_DX12::setShader(const std::string & shaderFileName, ShaderType type)
{
    removeShader(type);
    shaderFileNames[type] = shaderFileName;
}

void Material_DX12::removeShader(ShaderType type)
{
    // Remove File Name
    {
        auto it = shaderFileNames.find(type);
        if (it != shaderFileNames.end()) shaderFileNames.erase(it);
    }

    // Remove Blob
    {
        auto it = blobs.find(type);
        if (it != blobs.end()) 
        {
            SafeRelease(it->second);
            blobs.erase(it);
        }
    }
}

void Material_DX12::setDiffuse(Color c)
{
}

int Material_DX12::compileMaterial(std::string & errString)
{
    static std::map<ShaderType, std::pair<const char*, const char*>> shaderSettings = {
        { ShaderType::VS, { "VSMain", "vs_5_1" }},
        { ShaderType::PS, { "PSMain", "ps_5_1" }},
    };

    for (auto type_fileName : shaderFileNames)
    {
        auto type = type_fileName.first;
        auto fileName = type_fileName.second;

        ID3DBlob* blob, * error;

        std::ifstream fin(fileName);
        std::string shaderCode((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
        fin.close();

        std::string defines;
        for (auto define : shaderDefines[type])
        {
            defines += define;
        }

        shaderCode = defines + shaderCode;

        /*/Write complete shader to file 
        std::ofstream fout(fileName + "_defines.hlsl");
        fout << shaderCode;
        fout.close(); //*/

        UINT compileFlags = 0;
        #ifdef _DEBUG
        compileFlags |= D3DCOMPILE_DEBUG;
        compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
        #endif

        BreakOnFail2(D3DCompile(
            shaderCode.c_str(),
            shaderCode.size(),
            fileName.c_str(),
            nullptr,
            nullptr,
            shaderSettings[type].first,
            shaderSettings[type].second,
            compileFlags,
            0,
            &blob,
            &error
        ), (char*)error->GetBufferPointer());

        blobs[type] = blob;
    }

    return 0;
}

void Material_DX12::addConstantBuffer(std::string name, unsigned int location)
{
    if (constantBuffers[location] != nullptr)
    {
        delete constantBuffers[location];
    }

    constantBuffers[location] = new ConstantBuffer_DX12(name, location);
}

void Material_DX12::updateConstantBuffer(const void * data, size_t size, unsigned int location)
{
    constantBuffers[location]->setData(data, size, nullptr, 0);
}

int Material_DX12::enable()
{
    return 0;
}

void Material_DX12::disable()
{
}

ID3DBlob * Material_DX12::getBlob(ShaderType type)
{
    return blobs[type];
}
