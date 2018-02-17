#ifndef DEVICE_DX12_H
#define DEVICE_DX12_H

#include <d3d12.h>

extern ID3D12Device*			Device;
extern ID3D12DescriptorHeap*	SceneDescHeap;
extern ID3D12DescriptorHeap*	SamplerDescHeap;

#endif // !DEVICE_DX12_H