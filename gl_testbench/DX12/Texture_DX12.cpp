#include "Texture_DX12.h"

#include "stb_image.h"

Texture_DX12::Texture_DX12(ID3D12GraphicsCommandList* graphicsCommandList, ID3D12DescriptorHeap* srvDescHeap)
{
    m_Texture               = nullptr;
    m_TextureUploadHeap     = nullptr;
    m_CommandList           = graphicsCommandList;    

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels           = 1;
    textureDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.Width               = 640;
    textureDesc.Height              = 640;
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

    // Loading texture with STBI
    int w, h, bpp;
    unsigned char* rgb = stbi_load(filename.c_str(), &w, &h, &bpp, STBI_rgb_alpha);
    if (rgb == nullptr)
    {
        fprintf(stderr, "Error loading texture file: %s\n", filename.c_str());
        return -1;
    }

    // Setting texture data
    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = rgb;
    textureData.RowPitch = w * bpp;
    textureData.SlicePitch = w * h * bpp; // not needed since 2D texture 

    UpdateSubresources(m_CommandList, m_Texture, m_TextureUploadHeap, 0, 0, 1, &textureData);

    stbi_image_free(rgb);
    return 0;
}

void Texture_DX12::bind(unsigned int slot)
{

}