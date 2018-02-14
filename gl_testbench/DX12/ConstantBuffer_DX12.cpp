#include "ConstantBuffer_DX12.h"
#include "DX12Renderer.h"

ConstantBuffer_DX12::ConstantBuffer_DX12(UINT location)
	: ConstantBuffer_DX12("", location, DX12Renderer::m_Device, DX12Renderer::m_sceneDescriptorHeap)
{
}

ConstantBuffer_DX12::ConstantBuffer_DX12(std::string NAME, unsigned int location, ID3D12Device* device, ID3D12DescriptorHeap* cbvDescHeap)
{
	this->location = location;
    m_ConstantBuffer                = nullptr;
    m_pCbvDataBegin                 = nullptr;
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
	if (!m_ConstantBuffer)
	{
		create(size);
	}

    memcpy(m_pCbvDataBegin, data, size);
}

void ConstantBuffer_DX12::bind(Material* material)
{
	DX12Renderer::m_GraphicsCommandList->SetGraphicsRootConstantBufferView(location, m_ConstantBuffer->GetGPUVirtualAddress());
}

void ConstantBuffer_DX12::create(size_t size)
{
	ThrowIfFailed(DX12Renderer::m_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((size + 255) & ~255),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_ConstantBuffer)
	));

	// Map and initialize the constant buffer. We don't unmap this until the
	// app closes. Keeping things mapped for the lifetime of the resource is okay.
	CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
	ThrowIfFailed(m_ConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin)));
	
	D3D12_GPU_VIRTUAL_ADDRESS       gpuAddress = m_ConstantBuffer->GetGPUVirtualAddress();

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.SizeInBytes = (size + 255) & ~255;
	cbvDesc.BufferLocation = gpuAddress;

	//    printf("Creating Constant Buffer Nr: %d at GPU: %p & CPU: %p\n", counter, gpuAddress, cpuHandle.ptr);
	DX12Renderer::m_Device->CreateConstantBufferView(&cbvDesc, DX12Renderer::m_sceneDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

}
