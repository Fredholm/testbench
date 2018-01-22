#ifndef CONSTANTBUFFER_DX12_H
#define CONSTANTBUFFER_DX12_H

#include <d3d12.h>
#include <d3dx12.h>
#include "../ConstantBuffer.h"
#include "Utility.h"
#include <DirectXMath.h>

struct SceneConstantBuffer
{
    DirectX::XMFLOAT4 offset;
};

class ConstantBuffer_DX12 : public ConstantBuffer
{
public:
    ConstantBuffer_DX12(std::string NAME, unsigned int location, ID3D12Device* device, ID3D12DescriptorHeap* descHeap);
    ~ConstantBuffer_DX12();
    void setData(const void* data, size_t size, Material* material, unsigned int location);
    void bind(Material* material);

private:
    ID3D12DescriptorHeap* m_pDescHeap;
    ID3D12Device* m_pDevice;
    ID3D12Resource* m_ConstantBuffer;
    void* m_pConstantBufferDataBegin;
};

#endif // !CONSTANTBUFFER_DX12_H
