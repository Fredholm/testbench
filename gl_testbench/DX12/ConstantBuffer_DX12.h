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
    ConstantBuffer_DX12(std::string NAME, unsigned int location, ID3D12Device* device, ID3D12DescriptorHeap* cbvDescHeap);
    ~ConstantBuffer_DX12();
    void setData(const void* data, size_t size, Material* material, unsigned int location);
    void bind(Material* material);

	UINT GetLocationInHeap() { return m_LocationInHeap; }

private:
	UINT m_LocationInHeap;
    ID3D12Resource* m_ConstantBuffer;
    SceneConstantBuffer m_ConstantData;
    UINT8* m_pCbvDataBegin;
};

#endif // !CONSTANTBUFFER_DX12_H
