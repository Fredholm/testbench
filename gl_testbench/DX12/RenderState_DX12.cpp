#include "RenderState_DX12.h"
#include "Device_DX12.h"
#include <comdef.h>

RenderState_DX12::RenderState_DX12(ID3D12RootSignature* rootsignature)
    : m_pRootSignature(rootsignature), m_NeedToRebuild(false), m_WireFrame(false), m_PipelineState(nullptr)
{
	m_PixelShader = nullptr;
	m_VertexShader = nullptr;
}

RenderState_DX12::~RenderState_DX12() 
{
    deallocate();
}

void RenderState_DX12::recreate()
{
    deallocate();

	// It returns here if we're not ready to compile it yet, ex: setting wireframe and then compiling
	if (!m_VertexShader || !m_PixelShader)
		return;

#if defined(_DEBUG)
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
    pipelineStateDesc.pRootSignature = m_pRootSignature;
    pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(m_VertexShader);
    pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(m_PixelShader);

	CD3DX12_RASTERIZER_DESC rasterizer(D3D12_DEFAULT);
	rasterizer.FillMode = m_WireFrame ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
//	rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON;

    pipelineStateDesc.RasterizerState = rasterizer;
    pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
    pipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
    pipelineStateDesc.SampleMask = UINT_MAX;
    pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateDesc.NumRenderTargets = 1;
    pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pipelineStateDesc.SampleDesc.Count = 1;
	
    ThrowIfFailed(Device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&m_PipelineState)));
}

void RenderState_DX12::deallocate()
{
    if (m_PipelineState)
    {
        m_PipelineState->Release();
        m_PipelineState = nullptr;
    }
}

void RenderState_DX12::setWireFrame(bool wireFrameOn)
{
    m_WireFrame = wireFrameOn;
    recreate();
}

void RenderState_DX12::set() 
{
	recreate();
}

ID3D12PipelineState * RenderState_DX12::GetPipelineState()
{
    return m_PipelineState;
}