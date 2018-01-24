#include "Texture_DX12.h"

Texture_DX12::Texture_DX12()
{
    m_Texture = nullptr;

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
}

Texture_DX12::~Texture_DX12()
{
    if (m_Texture)
    {
        m_Texture->Release();
        m_Texture = nullptr;
    }
}

int Texture_DX12::loadFromFile(std::string filename)
{
    return 0;
}

void Texture_DX12::bind(unsigned int slot)
{
}

std::vector<UINT8> Texture_DX12::GenerateTextureData()
{
    return std::vector<UINT8>();
}
