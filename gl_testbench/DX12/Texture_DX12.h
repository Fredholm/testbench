#ifndef TEXTURE_DX12_H
#define TEXTURE_DX12_H

#include <d3d12.h>
#include <d3dx12.h> 
#include <vector>
#include "../Texture2D.h"
#include "Utility.h"
#include "Device_DX12.h"

class Texture_DX12 : public Texture2D
{
public:

    static const UINT TextureWidth      = 256;
    static const UINT TextureHeight     = 256;
    static const UINT TexturePixelSize  = 4;	

    Texture_DX12(ID3D12GraphicsCommandList* graphicsCommandList, ID3D12DescriptorHeap* srvDescHeap);
    virtual ~Texture_DX12();

    // returns 0 if texture was loaded.
    int loadFromFile(std::string filename);

    // bind texture to be used in the pipeline, binding to
    // slot "slot" in the active fragment shader.
    // slot can have different interpretation depending on the API.
    void bind(unsigned int slot);

private:
    std::vector<UINT8> GenerateTextureData();

    ID3D12Resource* m_Texture;
    ID3D12Resource* m_TextureUploadHeap;
};

#endif // !TEXTURE_DX12_H