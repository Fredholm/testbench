#ifndef RENDERSTATE_DX12_H
#define RENDERSTATE_DX12_H

#include "../RenderState.h"
#include "Utility.h"
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <d3dx12.h> // DX12 helper functions and structs

#pragma comment(lib,"D3D12.lib")
#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"D3DCompiler.lib")

class RenderState_DX12 : public RenderState
{
public:
    RenderState_DX12(ID3D12RootSignature* rootsignature);
    ~RenderState_DX12();

    void setWireFrame(bool wireFrameOn);
    void set();

    ID3D12PipelineState* GetPipelineState();
    bool isWireFrame() { return m_WireFrame; }

private:

    void recreate();
    void deallocate();

    bool m_WireFrame;
    bool m_NeedToRebuild;
    ID3D12RootSignature* m_pRootSignature;
    ID3D12PipelineState* m_PipelineState;
};


#endif // !RENDERSTATE_DX12_H
