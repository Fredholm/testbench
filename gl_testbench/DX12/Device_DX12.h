#ifndef DEVICE_DX12_H
#define DEVICE_DX12_H

#include <d3d12.h>

extern ID3D12Device* m_Device; //< Used for a bunch of things, there's no DeviceContext anymore either, (located in DX12/Device_DX12.h)
                                //   We use it as global because of the important function: CreateCommited/Reserved Resource which is needed pretty much everywhere

#endif // !DEVICE_DX12_H