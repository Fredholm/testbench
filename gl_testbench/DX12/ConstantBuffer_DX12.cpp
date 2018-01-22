#include "ConstantBuffer_DX12.h"

ConstantBuffer_DX12::ConstantBuffer_DX12(std::string NAME, unsigned int location, ID3D12Device* device, ID3D12DescriptorHeap* descHeap)
{
    m_ConstantBuffer                = nullptr;
    m_pCbvDataBegin                 = nullptr;
    m_pDevice                       = device;

    m_ConstantData.offset = DirectX::XMFLOAT4(1.f, 2.f, 3.f, 4.f);

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_ConstantBuffer)));

    // Describe and create a constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = (sizeof(SceneConstantBuffer) + 255) & ~255;
    m_pDevice->CreateConstantBufferView(&cbvDesc, descHeap->GetCPUDescriptorHandleForHeapStart());

    // Map and initialize the constant buffer. We don't unmap this until the
    // app closes. Keeping things mapped for the lifetime of the resource is okay.
    CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
    ThrowIfFailed(m_ConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin)));
    memcpy(m_pCbvDataBegin, &m_ConstantData, sizeof(m_ConstantData));

}

ConstantBuffer_DX12::~ConstantBuffer_DX12()
{
    if (m_ConstantBuffer)
    {
        m_ConstantBuffer->Release();
        m_ConstantBuffer = nullptr;
    }
}

void ConstantBuffer_DX12::setData(const void * data, size_t size, Material * material, unsigned int location)
{
    m_ConstantData.offset.x = 100;
    m_ConstantData.offset.y = 100;
    m_ConstantData.offset.z = 100;
    m_ConstantData.offset.w = 100;
    memcpy(m_pCbvDataBegin, &m_ConstantData, sizeof(m_ConstantData));
}

void ConstantBuffer_DX12::bind(Material* material)
{

}
