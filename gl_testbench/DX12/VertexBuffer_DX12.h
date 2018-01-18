#ifndef VERTEXBUFFER_DX12_H
#define VERTEXBUFFER_DX12_H

#include <DirectXMath.h>
#include "../VertexBuffer.h"

#define _USE_MATH_DEFINES

using namespace DirectX;

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT4 color;
};

class VertexBuffer_DX12 : public VertexBuffer
{
public:

public:
    VertexBuffer_DX12();
    ~VertexBuffer_DX12();
    void setData(const void* data, size_t size, size_t offset);
    void bind(size_t offset, size_t size, unsigned int location);
    void unbind();
    size_t getSize();
};

#endif // !VERTEXBUFFER_DX12_H
