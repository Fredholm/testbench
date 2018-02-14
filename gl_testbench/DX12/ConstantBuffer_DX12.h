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
	ConstantBuffer_DX12(UINT location);
    ConstantBuffer_DX12(std::string NAME, unsigned int location, ID3D12Device* device, ID3D12DescriptorHeap* cbvDescHeap);
    ~ConstantBuffer_DX12();
    void setData(const void* data, size_t size, Material* material = nullptr, unsigned int location = 0);
    void bind(Material* material = nullptr);

private:
	UINT location;
    ID3D12Resource* m_ConstantBuffer;
    UINT8* m_pCbvDataBegin;

	void create(size_t size);
};

#endif // !CONSTANTBUFFER_DX12_H
