#include "VertexBuffer_DX12.h"
#include <Windowsnumerics.h>

VertexBuffer_DX12::VertexBuffer_DX12(size_t size, VertexBuffer::DATA_USAGE usage)
{
    this->m_Size = size;

    // \note Using Heap Upload to transfer static data is not good or fast.
    //  We're are using it in this project for simplicity (and the guide that I followed along used it ^.^)
    ThrowIfFailed(m_Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(size),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_VertexBuffer)
    ));

    // Making spot for the Vertex Buffer
    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_VertexDataStart)));

    printf("Created Vertex Buffer %p with a size in bytes of: %d\n", this, size);
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
    printf("Adding data to VertexBuffer at %p -.- Size: %d, Offset: %d\n", this, size, offset);

    // Copy the triangle data to the place of the vertex buffer
    memcpy(m_VertexDataStart + offset, data, size);

	// Remember! when bind() is getting called later on, reenable this!
	m_VertexBuffer->Unmap(0, nullptr);

    // Initialize the Vertex Buffer View
    m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
    m_VertexBufferView.StrideInBytes = size / 3; // (float4 comes in at 48 bytes, it works when it's 16 bytes here, same with float2, which comes in at 16 which is only 8 bytes) i don't know..
    m_VertexBufferView.SizeInBytes = size;
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
