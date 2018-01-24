#include "VertexBuffer_DX12.h"
#include <Windowsnumerics.h>

VertexBuffer_DX12::VertexBuffer_DX12(size_t size, VertexBuffer::DATA_USAGE usage, ID3D12Device* device)
{
    this->m_Size = size;

    // \note Using Heap Upload to transfer static data is not good or fast.
    //  We're are using it in this project for simplicity (and the guide that I followed along used it ^.^)
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(m_Size * 2),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_VertexBuffer)
    ));

    // Making spot for the Vertex Buffer
    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_VertexDataStart)));
}

VertexBuffer_DX12::~VertexBuffer_DX12() 
{
    if (m_VertexBuffer)
    {
		m_VertexBuffer->Unmap(0, nullptr);
        m_VertexBuffer->Release();
        m_VertexBuffer = nullptr;
    }
}

void VertexBuffer_DX12::setData(const void * data, size_t size, size_t offset)
{
    // Copy the triangle data to the place of the vertex buffer
    memcpy(m_VertexDataStart + offset, data, size);

	// Remember! when bind() is getting called later on, reenable this!
	//   m_VertexBuffer->Unmap(0, nullptr);

    // Initialize the Vertex Buffer View
    m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress() + offset;
    m_VertexBufferView.StrideInBytes = (size / 3);
    m_VertexBufferView.SizeInBytes = m_Size;
}

void VertexBuffer_DX12::bind(size_t offset, size_t size, unsigned int location)
{
	printf("ya");
    CD3DX12_RANGE readRange(0, size);
    ThrowIfFailed(m_VertexBuffer->Map(location, &readRange, reinterpret_cast<void**>(m_VertexBufferView.BufferLocation + offset)));
}

void VertexBuffer_DX12::unbind()
{
    m_VertexBuffer->Unmap(0, nullptr);
}

size_t VertexBuffer_DX12::getSize()
{
    return size_t(m_Size);
}
