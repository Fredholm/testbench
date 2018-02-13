#include "Texture_DX12.h"

Texture_DX12::Texture_DX12(ID3D12GraphicsCommandList* graphicsCommandList, ID3D12DescriptorHeap* srvDescHeap)
{
    m_Texture               = nullptr;
    m_TextureUploadHeap     = nullptr;
    m_CommandList           = graphicsCommandList;    

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels           = 1;
    textureDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.Width               = TextureWidth;
    textureDesc.Height              = TextureHeight;
    textureDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize    = 1;
    textureDesc.SampleDesc.Count    = 1;
    textureDesc.SampleDesc.Quality  = 0;
    textureDesc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    // Creates the 2D Texture
    ThrowIfFailed(m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_Texture)));

    // Describe and create a SRV for the texture.
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    printf("Creating Shader Resource Nr: 1 at GPU: %p & CPU: %p\n", m_Texture->GetGPUVirtualAddress(), srvDescHeap->GetCPUDescriptorHandleForHeapStart().ptr);
    m_Device->CreateShaderResourceView(m_Texture, &srvDesc, srvDescHeap->GetCPUDescriptorHandleForHeapStart());
}

Texture_DX12::~Texture_DX12()
{
    if (m_Texture)
    {
        m_Texture->Release();
        m_Texture = nullptr;
    }

    if (m_TextureUploadHeap)
    {
        m_TextureUploadHeap->Release();
        m_TextureUploadHeap = nullptr;
    }
}

int Texture_DX12::loadFromFile(std::string filename)
{
    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Texture, 0, 1);

    // Create the GPU upload buffer.
    ThrowIfFailed(m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_TextureUploadHeap)));

    // We're not loading from file atm, we create our own texture right here.
    std::vector<UINT8> texture = GenerateTextureData();

    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = &texture[0];
    textureData.RowPitch = TextureWidth * TexturePixelSize;
    textureData.SlicePitch = textureData.RowPitch * TextureHeight;

    UpdateSubresources(m_CommandList, m_Texture, m_TextureUploadHeap, 0, 0, 1, &textureData);
    m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    return 0;
}

void Texture_DX12::bind(unsigned int slot)
{

}

// Creates a checkboard of check board with two colors (Taken from Microsoft Sample!)
std::vector<UINT8> Texture_DX12::GenerateTextureData()
{
    const UINT rowPitch = TextureWidth * TexturePixelSize;
    const UINT cellPitch = rowPitch >> 3;		// The width of a cell in the checkboard texture.
    const UINT cellHeight = TextureWidth >> 3;	// The height of a cell in the checkerboard texture.
    const UINT textureSize = rowPitch * TextureHeight;

    std::vector<UINT8> data(textureSize);
    UINT8* pData = &data[0];

    for (UINT n = 0; n < textureSize; n += TexturePixelSize)
    {
        UINT x = n % rowPitch;
        UINT y = n / rowPitch;
        UINT i = x / cellPitch;
        UINT j = y / cellHeight;

        if (i % 2 == j % 2)
        {
            pData[n]        = 255;	// R
            pData[n + 1]    = 0;	// G
            pData[n + 2]    = 255;	// B
            pData[n + 3]    = 255;	// A
        }
        else
        {
            pData[n]        = 0;	// R
            pData[n + 1]    = 255;	// G
            pData[n + 2]    = 255;	// B
            pData[n + 3]    = 255;	// A
        }
    }

    return data;
}
