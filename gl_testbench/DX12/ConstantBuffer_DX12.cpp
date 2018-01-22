#include "ConstantBuffer_DX12.h"

ConstantBuffer_DX12::ConstantBuffer_DX12(std::string NAME, unsigned int location, ID3D12Device* device, ID3D12DescriptorHeap* descHeap)
{
    m_ConstantBuffer                = nullptr;
    m_pConstantBufferDataBegin      = nullptr;
    m_pDevice                       = device;
    m_pDescHeap                     = descHeap;

    const UINT constantBufferSize = (sizeof(SceneConstantBuffer) + (256 - 1)) & ~(256 - 1); // must be a multiple 256 bytes
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_ConstantBuffer)));
}

ConstantBuffer_DX12::~ConstantBuffer_DX12()
{
    if (m_ConstantBuffer)
    {
        delete m_ConstantBuffer;
        m_ConstantBuffer = nullptr;
    }
}

void ConstantBuffer_DX12::setData(const void * data, size_t size, Material * material, unsigned int location)
{
    // Describe and create a constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = (sizeof(SceneConstantBuffer) + 255) & ~255;	// CB size is required to be 256-byte aligned.
    m_pDevice->CreateConstantBufferView(&cbvDesc, m_pDescHeap->GetCPUDescriptorHandleForHeapStart());

    // Map and initialize the constant buffer. We don't unmap this until the
    // app closes. Keeping things mapped for the lifetime of the resource is okay.
    CD3DX12_RANGE readRange(0, size);		// We do not intend to read from this resource on the CPU.
    ThrowIfFailed(m_ConstantBuffer->Map(0, &readRange, &m_pConstantBufferDataBegin));
    memcpy(m_pConstantBufferDataBegin, &data, size);
}

void ConstantBuffer_DX12::bind(Material* material)
{

}
