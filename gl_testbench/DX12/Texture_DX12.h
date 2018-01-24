#ifndef MESH_DX12_H
#define MESH_DX12_H

#include <d3d12.h>
#include <vector>
#include "../Texture2D.h"

class Texture_DX12 : public Texture2D
{
public:
    Texture_DX12();
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
};

#endif // !MESH_DX12_H