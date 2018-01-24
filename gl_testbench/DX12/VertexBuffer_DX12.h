#ifndef VERTEXBUFFER_DX12_H
#define VERTEXBUFFER_DX12_H

#include <Windows.h>
#include <d3d12.h>
#include <d3dx12.h> // DX12 helper functions and structs
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include "../VertexBuffer.h"
#include "Utility.h"
#include "Device_DX12.h"

#pragma comment(lib,"D3D12.lib")
#pragma comment(lib,"DXGI.lib")

#define _USE_MATH_DEFINES

using namespace DirectX;

struct Vertex
{
    XMFLOAT4 position;
};

class VertexBuffer_DX12 : public VertexBuffer
{
public:
    VertexBuffer_DX12(size_t size, VertexBuffer::DATA_USAGE usage);
    ~VertexBuffer_DX12();
    void setData(const void* data, size_t size, size_t offset);
    void bind(size_t offset, size_t size, unsigned int location);
    void unbind();
    size_t getSize();

    ID3D12Resource* getVertexBuffer() { return m_VertexBuffer; }
    D3D12_VERTEX_BUFFER_VIEW* getVertexBufferView() { return &m_VertexBufferView; }

private:
    size_t m_Size;
    ID3D12Resource* m_VertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
    UINT8* m_VertexDataStart;
};

#endif // !VERTEXBUFFER_DX12_H
