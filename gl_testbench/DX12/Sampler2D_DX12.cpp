#include "Sampler2D_DX12.h"
#include "Device_DX12.h"
#include <d3dx12.h>

Sampler2D_DX12::Sampler2D_DX12() : m_LocationInHeap(-1)
{
	// Getting the unique location in the hepap
	static int counter = 0;
	m_LocationInHeap = counter;
	counter++;

	// Default Values
	m_SamplerDesc.Filter			= D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	m_SamplerDesc.AddressU			= D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	m_SamplerDesc.AddressV			= D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	m_SamplerDesc.AddressW			= D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	m_SamplerDesc.MinLOD			= 0;
	m_SamplerDesc.MaxLOD			= D3D12_FLOAT32_MAX;
	m_SamplerDesc.MipLODBias		= 0.0f;
	m_SamplerDesc.MaxAnisotropy		= 1;
	m_SamplerDesc.ComparisonFunc	= D3D12_COMPARISON_FUNC_ALWAYS;
	m_SamplerDesc.BorderColor[0]	= D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;

	recreate();
}

Sampler2D_DX12::~Sampler2D_DX12() { }

void Sampler2D_DX12::setMagFilter(FILTER filter) { }

void Sampler2D_DX12::setMinFilter(FILTER filter) { }

void Sampler2D_DX12::setWrap(WRAPPING s, WRAPPING t) 
{
	// Adress U outside 0:1
	if		(s == REPEAT)	m_SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	else if (s == CLAMP)	m_SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	// Adress V outside 0:1
	if		(t == REPEAT)	m_SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	else if (t == CLAMP)	m_SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	recreate();
}

void Sampler2D_DX12::recreate()
{
	// Get the sampler descriptor size for the current device.
	const UINT samplerDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	// Get a handle to the start of the descriptor heap.
	CD3DX12_CPU_DESCRIPTOR_HANDLE samplerHandle(SamplerDescHeap->GetCPUDescriptorHandleForHeapStart());
	samplerHandle.Offset(m_LocationInHeap, samplerDescriptorSize);

	Device->CreateSampler(&m_SamplerDesc, samplerHandle);
}