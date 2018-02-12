#include "RenderState_DX12.h"
#include "Device_DX12.h"

RenderState_DX12::RenderState_DX12(ID3D12RootSignature* rootsignature)
    : m_pRootSignature(rootsignature), m_NeedToRebuild(false), m_WireFrame(false), m_PipelineState(nullptr)
{
    recreate();
}

RenderState_DX12::~RenderState_DX12() 
{

}

void RenderState_DX12::recreate()
{
    deallocate();

    ID3DBlob* vertexShader;
    ID3DBlob* pixelShader;

#if defined(_DEBUG)
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ID3DBlob* errorMsg;

    ThrowIfFailed(D3DCompileFromFile(L"../assets/DX12/shader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &errorMsg));
    ThrowIfFailed(D3DCompileFromFile(L"../assets/DX12/shader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &errorMsg));

    D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // 16
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },  // 32
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }       // 40
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
    pipelineStateDesc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
    pipelineStateDesc.pRootSignature = m_pRootSignature;
    pipelineStateDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader);
    pipelineStateDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader);
    pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
    pipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
    pipelineStateDesc.SampleMask = UINT_MAX;
    pipelineStateDesc.PrimitiveTopologyType = m_WireFrame ? D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE : D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateDesc.NumRenderTargets = 1;
    pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pipelineStateDesc.SampleDesc.Count = 1;
    ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&m_PipelineState)));
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

void RenderState_DX12::set() { }

ID3D12PipelineState * RenderState_DX12::GetPipelineState()
{
    return m_PipelineState;
}