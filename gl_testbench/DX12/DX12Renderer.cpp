#include "DX12Renderer.h"

#define NAME_D3D12_OBJECT(x) SetName(x.Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed(x[n].Get(), L#x, n)

DX12Renderer::DX12Renderer()
{
    m_SDLWindow             = nullptr;
    m_SwapChain             = nullptr;
    m_Device                = nullptr;
    m_CommandQueue          = nullptr;
    m_RootSignature         = nullptr;
    m_cbDescriptorHeap      = nullptr;
    m_rtDescriptorHeap      = nullptr;
    m_PipelineState         = nullptr;
    m_GraphicsCommandList   = nullptr;
    m_Fence                 = nullptr;
    m_CommandAllocator      = nullptr;
    m_FenceValue            = NULL;

    for (UINT i = 0; i < Options::FrameCount; i++)
    {
        m_RenderTargets[i] = nullptr;
    }
}

DX12Renderer::~DX12Renderer() { }

Mesh* DX12Renderer::makeMesh()
{
    return (Mesh*)new Mesh_DX12();
}

Texture2D * DX12Renderer::makeTexture2D()
{
    //	return (Texture2D*)new Texture2D_DX12();
    return nullptr;
}

Sampler2D * DX12Renderer::makeSampler2D()
{
    //	return (Sampler2D*)new Sampler2D_DX12();;
    return nullptr;
}

ConstantBuffer * DX12Renderer::makeConstantBuffer(std::string NAME, unsigned int location)
{
    return new ConstantBuffer_DX12(NAME, location, m_Device, m_cbDescriptorHeap);
}

RenderState * DX12Renderer::makeRenderState()
{
    //RenderState_DX12* newRS = new RenderState_DX12();
    //newRS->setGlobalWireFrame(&this->globalWireframeMode);
    //newRS->setWireFrame(false);
    //return (RenderState*)newRS;
    return nullptr;
}

std::string DX12Renderer::getShaderPath()
{
    return std::string("..\\assets\\DX12\\");
}

std::string DX12Renderer::getShaderExtension()
{
    return std::string(".hlsl");
}

VertexBuffer * DX12Renderer::makeVertexBuffer(size_t size, VertexBuffer::DATA_USAGE usage)
{
    return new VertexBuffer_DX12(size, usage, m_Device);
}

Material * DX12Renderer::makeMaterial(const std::string & name)
{
    //  return (Material*)new Material_DX12();;
    return nullptr;
}

Technique * DX12Renderer::makeTechnique(Material* material, RenderState* renderState)
{
    Technique* t = new Technique(material, renderState);
    return t;
}

int DX12Renderer::initialize(unsigned int width, unsigned int height)
{
    // Initialize Window
    initSDLWindow(width, height);

    // Initialize DX12
    initDX12(width, height);

    return 0;
}

void DX12Renderer::initSDLWindow(unsigned int width, unsigned int height)
{
    // Initializing SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "%s", SDL_GetError());
        exit(-1);
    }

    // Create the SDL window
    m_SDLWindow = SDL_CreateWindow("DirectX12", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    // Get Window Information
    struct SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (-1 == SDL_GetWindowWMInfo(m_SDLWindow, &wmInfo))
        throw std::runtime_error("Couldn't get WM Info!");

    // Get the HWND and HDC from SDL window
    m_WindowHandle = wmInfo.info.win.window;
    m_WindowContext = wmInfo.info.win.hdc;
}

void DX12Renderer::initDX12(unsigned int width, unsigned int height)
{
    loadPipeline(width, height);
    loadAssets();
}

void DX12Renderer::loadPipeline(unsigned int width, unsigned int height)
{
    // Setting start variables
    m_RenderTargetViewDescSize      = 0;
    m_ConstantBufferViewDescSize    = 0;
    m_FrameIndex                    = 0;
    m_Viewport                      = CD3DX12_VIEWPORT(0.0f, 0.0f, width, height);
    m_ScissorRect                   = CD3DX12_RECT(0, 0, LONG(width), LONG(height));
    m_AspectRatio                   = float(width) / float(height);

    /************************/
    /*  Loading Pipeline    */
    /************************/

    /*
        Creation of the Device
    */

    // Collect flags from different stuff for creation of a correct factory
    UINT factoryFlags = 0;

    // Enable the debug layer
    ID3D12Debug* debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
        factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }

    IDXGIFactory4* factory;
    ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)));

    IDXGIAdapter1* hardwareAdapter;
    getHardwareAdapter(factory, &hardwareAdapter);

    ThrowIfFailed(D3D12CreateDevice(hardwareAdapter, Options::FeatureLevel, IID_PPV_ARGS(&m_Device)));

    /*
        Creation of the Command Queue
    */

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {}; // Reset
    commandQueueDesc.Flags  = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.Type   = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_CommandQueue)));

    /*
        Creation of the Swap Chain
    */

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount           = Options::FrameCount;
    swapChainDesc.Width                 = width;
    swapChainDesc.Height                = height;
    swapChainDesc.Format                = Options::Format;
    swapChainDesc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count      = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(m_CommandQueue, m_WindowHandle, &swapChainDesc, nullptr, nullptr, &swapChain));

    // We make ALT+Enter unavailable because the swap chain doesn't support fullscreen transitions
    ThrowIfFailed(factory->MakeWindowAssociation(m_WindowHandle, DXGI_MWA_NO_ALT_ENTER));

    m_SwapChain = (IDXGISwapChain3*)(swapChain.Get());
    m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

    /*
        Creation of the Descriptor Heap
    */

    // Render Targets
    D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc = {};
    renderTargetViewHeapDesc.NumDescriptors = Options::FrameCount;
    renderTargetViewHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    renderTargetViewHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(m_Device->CreateDescriptorHeap(&renderTargetViewHeapDesc, IID_PPV_ARGS(&m_rtDescriptorHeap)));

    // Describe and create a constant buffer view (CBV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC constantBufferHeapdesc  = {};
    constantBufferHeapdesc.NumDescriptors   = 1;
    constantBufferHeapdesc.Type             = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    constantBufferHeapdesc.Flags            = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  // Can be bound to the pipeline 
    ThrowIfFailed(m_Device->CreateDescriptorHeap(&constantBufferHeapdesc, IID_PPV_ARGS(&m_cbDescriptorHeap)));

    m_RenderTargetViewDescSize		= m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_ConstantBufferViewDescSize	= m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    /*
        Creation of Render Target
    */

    CD3DX12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle(m_rtDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < Options::FrameCount; i++)
    {
        ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])));
        m_Device->CreateRenderTargetView(m_RenderTargets[i], nullptr, renderTargetViewHandle);
        renderTargetViewHandle.Offset(1, m_RenderTargetViewDescSize);
    }

    /*
        Creation of Command Allocater resources
    */

    ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));


    // These things won't be used again
    hardwareAdapter->Release();
    hardwareAdapter = nullptr;
    factory->Release();
    factory = nullptr;
}

void DX12Renderer::loadAssets()
{
    /*******************************************/
    /*  Loading Resources Needed for assets    */
    /*******************************************/

    /*
        Creation of EMPTY Root Signature
    */
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

    // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

    CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
    CD3DX12_ROOT_PARAMETER1 rootParameters[1];

    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);

    // Allow input layout and deny uneccessary access to certain pipeline stages.
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

    ID3DBlob* signature;
    ID3DBlob* error;
    ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
    ThrowIfFailed(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));

    /*
        Creation of Shaders (Move this over to Material_DX12 asap)
    */
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
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    /*
        Creation of Graphical Pipeline State
    */

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc    = {};
    pipelineStateDesc.InputLayout                           = { inputElementDesc, _countof(inputElementDesc) };
    pipelineStateDesc.pRootSignature                        = m_RootSignature;
    pipelineStateDesc.VS                                    = CD3DX12_SHADER_BYTECODE(vertexShader);
    pipelineStateDesc.PS                                    = CD3DX12_SHADER_BYTECODE(pixelShader);
    pipelineStateDesc.RasterizerState                       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pipelineStateDesc.BlendState                            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pipelineStateDesc.DepthStencilState.DepthEnable         = FALSE;
    pipelineStateDesc.DepthStencilState.StencilEnable       = FALSE;
    pipelineStateDesc.SampleMask                            = UINT_MAX;
    pipelineStateDesc.PrimitiveTopologyType                 = Options::Topology;
    pipelineStateDesc.NumRenderTargets                      = 1;
    pipelineStateDesc.RTVFormats[0]                         = Options::Format;
    pipelineStateDesc.SampleDesc.Count                      = 1;
    ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&m_PipelineState)));

    /*
        Creation of Command List
    */

    ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator, m_PipelineState, IID_PPV_ARGS(&m_GraphicsCommandList)));
    ThrowIfFailed(m_GraphicsCommandList->Close()); // Nothing to record atm, so we'll close it and then open it when ready, will crash if we leave it open for the GPU

    /*
        Creation of Objects used for Syncing GPU and CPU
    */
    
    ThrowIfFailed(m_Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
    m_FenceValue = 1;

    m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_FenceEvent == nullptr)
        ThrowWithMessage(0x80004003, __FILE__, __LINE__);

    // Setup is done, just for safety, we wait for the GPU, until we start the commands
    waitForTheGPU();
}

void DX12Renderer::getHardwareAdapter(IDXGIFactory2* factory, IDXGIAdapter1** adapter)
{
    IDXGIAdapter1* tempAdapter;

    // Reset the current 
    *adapter = nullptr;

    // Go through all the available hardware adapters that supports D3D12
    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &tempAdapter); ++i)
    {
        DXGI_ADAPTER_DESC1 desc;
        tempAdapter->GetDesc1(&desc);

        // If we only get these, we probably need to use WARP instead..
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

        // We found one!
        if (SUCCEEDED(D3D12CreateDevice(tempAdapter, Options::FeatureLevel, _uuidof(ID3D12Device), nullptr)))
            break;
    }

    // Change the current
    *adapter = tempAdapter;
}

void DX12Renderer::waitForTheGPU()
{
    // Send a signal to the command queue
    ThrowIfFailed(m_CommandQueue->Signal(m_Fence, m_FenceValue));

    // Wait until the fence (GPU is done) has been processed
    ThrowIfFailed(m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent));
    WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

    // Increase the fence value for the current frame
    m_FenceValue++;
}

void DX12Renderer::present()
{
    // Open for more command lists in the future
    ID3D12CommandList* pCommandLists[] = { m_GraphicsCommandList };
    m_CommandQueue->ExecuteCommandLists(_countof(pCommandLists), pCommandLists);

    // Present
    ThrowIfFailed(m_SwapChain->Present(1, 0));

    moveToNextFrame();

	waitForTheGPU();
}

void DX12Renderer::moveToNextFrame()
{
    // Issue a signal command into the command queue
    const UINT64 currentFenceValue = m_FenceValue;
    ThrowIfFailed(m_CommandQueue->Signal(m_Fence, currentFenceValue));

    // Change the frame index
    m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered, wait
    if (m_Fence->GetCompletedValue() < m_FenceValue)
    {
        ThrowIfFailed(m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent));
        WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
    }

    // Set the fence value for the next frame
    m_FenceValue = currentFenceValue + 1;
}

void DX12Renderer::setWinTitle(const char * title)
{
    SDL_SetWindowTitle(m_SDLWindow, title);
}

void DX12Renderer::setClearColor(float r, float g, float b, float a)
{
    m_ClearColor[0] = r;
    m_ClearColor[1] = g;
    m_ClearColor[2] = b;
    m_ClearColor[3] = a;
}

void DX12Renderer::clearBuffer(unsigned int flag)
{
    // Should not get reset when used on the GPU, the fence should probably reassure that doesn't happen (hopefully)
    ThrowIfFailed(m_CommandAllocator->Reset());
    ThrowIfFailed(m_GraphicsCommandList->Reset(m_CommandAllocator, m_PipelineState));

    ID3D12DescriptorHeap* ppHeaps[] = { m_cbDescriptorHeap };
    m_GraphicsCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	m_GraphicsCommandList->SetGraphicsRootSignature(m_RootSignature);

    m_GraphicsCommandList->RSSetViewports(1, &m_Viewport);
    m_GraphicsCommandList->RSSetScissorRects(1, &m_ScissorRect);

    // Tell the command list that we're using the backbuffer as the render target
    m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    CD3DX12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle(m_rtDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RenderTargetViewDescSize);
    m_GraphicsCommandList->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, nullptr);

    // Record some commands 
    m_GraphicsCommandList->ClearRenderTargetView(renderTargetViewHandle, m_ClearColor, 0, nullptr);


    // TODO
    /* Clear the non-existant depth buffer */
    // m_GraphicsCommandList->ClearRenderTargetView(renderTargetViewHandle, m_ClearColor, 0, nullptr);
}

void DX12Renderer::setRenderState(RenderState* ps)
{
}

void DX12Renderer::submit(Mesh* mesh)
{
    m_DrawList.push_back(mesh);
}

void DX12Renderer::frame()
{
    // Set the Topology
    m_GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Adding meshes to to drawing
    for (Mesh* mesh : m_DrawList)
    {
        size_t numberElements = mesh->geometryBuffers[0].numElements;
        for (size_t i = 0; i < numberElements; i++)
            m_GraphicsCommandList->IASetVertexBuffers(i, 1, static_cast<VertexBuffer_DX12*>(mesh->geometryBuffers[i].buffer)->getVertexBufferView());

		m_GraphicsCommandList->SetGraphicsRootDescriptorTable(0, static_cast<ConstantBuffer_DX12*>(mesh->txBuffer)->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	}

    // Draw all the meshes
    m_GraphicsCommandList->DrawInstanced(3, m_DrawList.size(), 0, 0);
    m_DrawList.clear();

    // Indicate that the back buffer will be present
    m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    ThrowIfFailed(m_GraphicsCommandList->Close());
}

int DX12Renderer::shutdown()
{
    waitForTheGPU();
    CloseHandle(m_FenceEvent);

    // Remove the SDL Window 
    SDL_GL_DeleteContext(m_WindowContext);
    SDL_Quit();

    if (m_SwapChain)
    {
        m_SwapChain->Release();
        m_SwapChain = nullptr;
    }

    if (m_Device)
    {
        m_Device->Release();
        m_Device = nullptr;
    }
   
    for (UINT i = 0; i < Options::FrameCount; i++)
    {
        if (m_RenderTargets[i])
        {
            // Fix please
            // m_RenderTargets[i]->Release();
            m_RenderTargets[i] = nullptr;
        }
    }

    if (m_CommandAllocator)
    {
        m_CommandAllocator->Release();
        m_CommandAllocator = nullptr;
    }

    if (m_CommandQueue)
    {
        m_CommandQueue->Release();
        m_CommandQueue = nullptr;
    }

    if (m_RootSignature)
    {
        m_RootSignature->Release();
        m_RootSignature = nullptr;
    }

    if (m_rtDescriptorHeap)
    {
        m_rtDescriptorHeap->Release();
        m_rtDescriptorHeap = nullptr;
    }

    if (m_cbDescriptorHeap)
    {
        m_cbDescriptorHeap->Release();
        m_cbDescriptorHeap = nullptr;
    }

    if (m_PipelineState)
    {
        m_PipelineState->Release();
        m_PipelineState = nullptr;
    }

    if (m_GraphicsCommandList)
    {
        m_GraphicsCommandList->Release();
        m_GraphicsCommandList = nullptr;
    }

    if (m_Fence)
    {
        m_Fence->Release();
        m_Fence = nullptr;
    }

    return 0;
}