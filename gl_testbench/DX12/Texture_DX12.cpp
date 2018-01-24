#include "Texture_DX12.h"

#define TEXTURE_WIDTH   256
#define TEXTURE_HEIGHT  256
#define TEXTURE_PIXEL   4   // Bytes used to represent a pixel in the texture

Texture_DX12::Texture_DX12(ID3D12GraphicsCommandList* graphicsCommandList, ID3D12DescriptorHeap* srvDescHeap)
{
    m_Texture               = nullptr;
    m_TextureUploadHeap     = nullptr;

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels           = 1;
    textureDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.Width               = TEXTURE_WIDTH;
    textureDesc.Height              = TEXTURE_HEIGHT;
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

    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Texture, 0, 1);

    // Create the GPU upload buffer.
    ThrowIfFailed(m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_TextureUploadHeap)));

    std::vector<UINT8> texture = GenerateTextureData();

    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = &texture[0];
    textureData.RowPitch = TEXTURE_WIDTH * TEXTURE_HEIGHT;
    textureData.SlicePitch = textureData.RowPitch * TEXTURE_HEIGHT;

    UpdateSubresources(graphicsCommandList, m_Texture, m_TextureUploadHeap, 0, 0, 1, &textureData);
    graphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    // Describe and create a SRV for the texture.
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    m_Device->CreateShaderResourceView(m_Texture, &srvDesc, srvDescHeap->GetCPUDescriptorHandleForHeapStart());
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

// Creates a checkboard of white & black (Taken from Microsoft Sample!)
std::vector<UINT8> Texture_DX12::GenerateTextureData()
{
    const UINT rowPitch = TEXTURE_WIDTH * TEXTURE_PIXEL;
    const UINT cellPitch = rowPitch >> 3;		// The width of a cell in the checkboard texture.
    const UINT cellHeight = TEXTURE_WIDTH >> 3;	// The height of a cell in the checkerboard texture.
    const UINT textureSize = rowPitch * TEXTURE_HEIGHT;

    std::vector<UINT8> data(textureSize);
    UINT8* pData = &data[0];

    for (UINT n = 0; n < textureSize; n += TEXTURE_PIXEL)
    {
        UINT x = n % rowPitch;
        UINT y = n / rowPitch;
        UINT i = x / cellPitch;
        UINT j = y / cellHeight;

        if (i % 2 == j % 2)
        {
            // Black
            pData[n] = 0x00;		// R
            pData[n + 1] = 0x00;	// G
            pData[n + 2] = 0x00;	// B
            pData[n + 3] = 0xff;	// A
        }
        else
        {
            // White
            pData[n] = 0xff;		// R
            pData[n + 1] = 0xff;	// G
            pData[n + 2] = 0xff;	// B
            pData[n + 3] = 0xff;	// A
        }
    }

    return data;
}
