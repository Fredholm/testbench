#include "VertexBuffer_DX12.h"

Vertex trianglesVertices[] =
{
    { { 0.0f, 0.25f, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
{ { 0.25f, -0.25f, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
{ { -0.25f, -0.25f, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }
};

const UINT vertexBufferSize = sizeof(trianglesVertices);

VertexBuffer_DX12::VertexBuffer_DX12(size_t size, VertexBuffer::DATA_USAGE usage, ID3D12Device* device)
{

    // \note Using Heap Upload to transfer static data is not good or fast.
    //  where are using it in this project for simplicity (and the guide that I followed along used it ^.^)
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_VertexBuffer)
    ));
}

VertexBuffer_DX12::~VertexBuffer_DX12() 
{
    if (m_VertexBuffer)
    {
        m_VertexBuffer->Release();
        m_VertexBuffer = nullptr;
    }
}

void VertexBuffer_DX12::setData(const void * data, size_t size, size_t offset)
{
    // Copy the triangle data to the vertex buffer
    UINT8* vertexDataStart;
    CD3DX12_RANGE readRange(0, 0); // Not intended for the CPU to read
    ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vertexDataStart)));
    memcpy(vertexDataStart, trianglesVertices, sizeof(trianglesVertices));
    m_VertexBuffer->Unmap(0, nullptr);

    // Initialize the Vertex Buffer View
    m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
    m_VertexBufferView.StrideInBytes = sizeof(Vertex);
    m_VertexBufferView.SizeInBytes = vertexBufferSize;
}

void VertexBuffer_DX12::bind(size_t offset, size_t size, unsigned int location)
{
    UINT8* vertexDataStart;
    CD3DX12_RANGE readRange(0, 0); // Not intended for the CPU to read
    ThrowIfFailed(m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vertexDataStart)));
    memcpy(vertexDataStart, trianglesVertices, sizeof(trianglesVertices));
}

void VertexBuffer_DX12::unbind()
{
    m_VertexBuffer->Unmap(0, nullptr);
}

size_t VertexBuffer_DX12::getSize()
{
    return size_t(vertexBufferSize);
}
