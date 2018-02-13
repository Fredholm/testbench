#include "ConstantBuffer_DX12.h"

ConstantBuffer_DX12::ConstantBuffer_DX12(std::string NAME, unsigned int location, ID3D12Device* device, ID3D12DescriptorHeap* cbvDescHeap)
{
    m_ConstantBuffer                = nullptr;
    m_pCbvDataBegin                 = nullptr;
    m_ConstantData.offset           = DirectX::XMFLOAT4(0, 0, 0, 0);

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(1024),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_ConstantBuffer)));

    ZeroMemory(&m_ConstantData, sizeof(m_ConstantData));
    // Map and initialize the constant buffer. We don't unmap this until the
    // app closes. Keeping things mapped for the lifetime of the resource is okay.
    CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
    ThrowIfFailed(m_ConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin)));

    CD3DX12_CPU_DESCRIPTOR_HANDLE   cpuHandle(cbvDescHeap->GetCPUDescriptorHandleForHeapStart());
    D3D12_GPU_VIRTUAL_ADDRESS       gpuAddress = m_ConstantBuffer->GetGPUVirtualAddress();

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.SizeInBytes = (sizeof(SceneConstantBuffer) + 255) & ~255;
    cbvDesc.BufferLocation = gpuAddress;

    const UINT cbvSrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    static int counter = 1;
    cpuHandle.Offset(counter, cbvSrvDescriptorSize);
    counter++;

//    printf("Creating Constant Buffer Nr: %d at GPU: %p & CPU: %p\n", counter, gpuAddress, cpuHandle.ptr);
    device->CreateConstantBufferView(&cbvDesc, cpuHandle);
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
	DirectX::XMFLOAT4 dataGet;
	memcpy(&dataGet, data, size);
    m_ConstantData.offset.x = dataGet.x;
	m_ConstantData.offset.y = dataGet.y;
    m_ConstantData.offset.z = -dataGet.z;

    memcpy(m_pCbvDataBegin, &m_ConstantData, sizeof(m_ConstantData));
}

void ConstantBuffer_DX12::bind(Material* material)
{

}
