#include "Material_DX12.h"
#include <exception>

Material_DX12::Material_DX12() 
    : Material()
{}

Material_DX12::~Material_DX12() {}

void Material_DX12::addConstantBuffer(std::string name, unsigned int location)
{
    if (constantBuffers.find(location) == constantBuffers.end())
    {
        constantBuffers[location] = new ConstantBuffer_DX12(,);
    }
    else
    {
        throw std::runtime_error("Constant Buffer slot in use!");
    }
}
