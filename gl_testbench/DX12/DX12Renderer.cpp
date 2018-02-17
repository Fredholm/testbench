#include "DX12Renderer.h"
#include "Texture_DX12.h"
#include "Technique_DX12.h"
#include "../IA.h"

#define ROOT_PARAMETER_POS			0
#define ROOT_PARAMETER_NOR			1
#define ROOT_PARAMETER_TEX			2
#define ROOT_PARAMETER_TRANSLATE	3
#define ROOT_PARAMETER_TEXTURE		4
#define ROOT_PARAMETER_DIFFUSE		5

#define NAME_D3D12_OBJECT(x) SetName(x.Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed(x[n].Get(), L#x, n)

DX12Renderer::DX12Renderer()
{
    m_SDLWindow             = nullptr;
    m_SwapChain             = nullptr;
    Device                = nullptr;
    m_CommandQueue          = nullptr;
    m_RootSignature         = nullptr;
    SceneDescHeap   = nullptr;
    m_rtDescriptorHeap      = nullptr;
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
    return (Texture2D*)new Texture_DX12(m_GraphicsCommandList, SceneDescHeap, m_CommandAllocator, m_CommandQueue, m_GraphicsCommandList, this);
}

Sampler2D * DX12Renderer::makeSampler2D()
{
    return (Sampler2D*)new Sampler2D_DX12();;
}

ConstantBuffer * DX12Renderer::makeConstantBuffer(std::string NAME, unsigned int location)
{
    return new ConstantBuffer_DX12(NAME, location);
}

RenderState * DX12Renderer::makeRenderState()
{
    RenderState_DX12* newRS = new RenderState_DX12(m_RootSignature);
    return (RenderState*)newRS;
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
    return new VertexBuffer_DX12(size, usage);
}

Material * DX12Renderer::makeMaterial(const std::string & name)
{
    return (Material*)new Material_DX12(name);
}

Technique * DX12Renderer::makeTechnique(Material* material, RenderState* renderState)
{
	return (Technique*)new Technique_DX12(material, renderState);
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
    m_CBV_SRV_UAV_Heap_Size         = 0;
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

    ThrowIfFailed(D3D12CreateDevice(hardwareAdapter, Options::FeatureLevel, IID_PPV_ARGS(&Device)));

    /*
        Creation of the Command Queue
    */

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {}; // Reset
    commandQueueDesc.Flags  = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueueDesc.Type   = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_CommandQueue)));

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
        Creation of the Descriptor Heaps
    */

    // Gets the description view sizes of each descriptor heaps
    m_RenderTargetViewDescSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_CBV_SRV_UAV_Heap_Size = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // Create the Render Target View (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc = {};
    renderTargetViewHeapDesc.NumDescriptors     = Options::FrameCount;
    renderTargetViewHeapDesc.Type               = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    renderTargetViewHeapDesc.Flags              = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(Device->CreateDescriptorHeap(&renderTargetViewHeapDesc, IID_PPV_ARGS(&m_rtDescriptorHeap)));

    // Create the Shader Resource View (SRV) descriptor heap & 
    // Create the Constant Buffer View (CBV) descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC constantBufferHeapDesc = {};
    constantBufferHeapDesc.NumDescriptors       = 105; // temp! one for each constant buffer (100) and one for the texture and 4 for diffuse color
    constantBufferHeapDesc.Type                 = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    constantBufferHeapDesc.Flags                = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  
    ThrowIfFailed(Device->CreateDescriptorHeap(&constantBufferHeapDesc, IID_PPV_ARGS(&SceneDescHeap)));

    /*
        Creation of Render Target
    */

    CD3DX12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle(m_rtDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < Options::FrameCount; i++)
    {
        ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])));
        Device->CreateRenderTargetView(m_RenderTargets[i], nullptr, renderTargetViewHandle);
        renderTargetViewHandle.Offset(1, m_RenderTargetViewDescSize);
    }

    /*
        Creation of Command Allocater resources
    */

    ThrowIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));


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

    if (FAILED(Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

    CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
    CD3DX12_ROOT_PARAMETER1 rootParameters[6];

	// Vertex Data
	rootParameters[ROOT_PARAMETER_POS].InitAsShaderResourceView(POSITION, 0);
	rootParameters[ROOT_PARAMETER_NOR].InitAsShaderResourceView(NORMAL, 0);
	rootParameters[ROOT_PARAMETER_TEX].InitAsShaderResourceView(TEXTCOORD, 0);

	// Diffuse
	ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, DIFFUSE_TINT, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	rootParameters[ROOT_PARAMETER_DIFFUSE].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_ALL);

	// Translation in a descriptor heap
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, TRANSLATION, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    rootParameters[ROOT_PARAMETER_TRANSLATE].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);

	// Texture in a descriptor heap
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, DIFFUSE_SLOT, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    rootParameters[ROOT_PARAMETER_TEXTURE].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_ALL);

    // Sampler (For texturing)
    D3D12_STATIC_SAMPLER_DESC sampler   = {};
    sampler.Filter                      = D3D12_FILTER_MIN_MAG_MIP_POINT;               // What Filtering Method should be used, List:(https://msdn.microsoft.com/en-us/library/windows/desktop/dn770367)
    sampler.AddressU                    = D3D12_TEXTURE_ADDRESS_MODE_BORDER;            // Outside the range (0:1) of U 
    sampler.AddressV                    = D3D12_TEXTURE_ADDRESS_MODE_BORDER;            // Outside the range (0:1) of V
    sampler.AddressW                    = D3D12_TEXTURE_ADDRESS_MODE_BORDER;            // Outside the range (0:1) of W
    sampler.MipLODBias                  = 0;                                            // Offset from the calculated mipmap level, Ex: Direct3D calculates mipmaplevel 5, MipLodBias is 2, this results in a mipmap level of 7
    sampler.MaxAnisotropy               = 0;                                            // Clamping value if filters ANISOTROPIC or COMPARISON_ANISOTROPIC was used (Valid between 1:16)
    sampler.ComparisonFunc              = D3D12_COMPARISON_FUNC_NEVER;                  // Function which compares sampled data against existing sampling data, List:(https://msdn.microsoft.com/en-us/library/windows/desktop/dn770349)
    sampler.BorderColor                 = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;       // If ADDRESS_MODE_BORDER was used somewhere, List(https://msdn.microsoft.com/en-us/library/windows/desktop/dn903815)
    sampler.MinLOD                      = 0.0f;                                         // Minimun clamping of mipmap (min: 0.f)        
    sampler.MaxLOD                      = D3D12_FLOAT32_MAX;                            // Maximum clamping of mipmap (max: D3D12_FLOAT32_MAX)
    sampler.ShaderRegister              = 0;                                            // Example: (HLSL) Texture2D<float4> a : register(t2, space3) -> ShaderRegister of 2 
    sampler.RegisterSpace               = 0;                                            // Example: (HLSL) Texture2D<float4> a : register(t2, space3) -> RegisterSpace of 3 
    sampler.ShaderVisibility            = D3D12_SHADER_VISIBILITY_ALL;                  // Which Shaders should be able to see this sampler

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ID3DBlob* signature;
    ID3DBlob* error;
    ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
    ThrowIfFailed(Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));

    /*
        Creation of Command List
    */

    ThrowIfFailed(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator, nullptr, IID_PPV_ARGS(&m_GraphicsCommandList)));
    ThrowIfFailed(m_GraphicsCommandList->Close()); // Nothing to record atm, so we'll close it and then open it when ready, will crash if we leave it open for the GPU

    /*
        Creation of Objects used for Syncing GPU and CPU
    */
    
    ThrowIfFailed(Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
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
    ThrowIfFailed(m_GraphicsCommandList->Reset(m_CommandAllocator, nullptr));

    m_GraphicsCommandList->SetGraphicsRootSignature(m_RootSignature);

    ID3D12DescriptorHeap* ppHeaps[] = { SceneDescHeap };
    m_GraphicsCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    m_GraphicsCommandList->RSSetViewports(1, &m_Viewport);
    m_GraphicsCommandList->RSSetScissorRects(1, &m_ScissorRect);

    // Tell the command list that we're using the backbuffer as the render target
    m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    CD3DX12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle(m_rtDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RenderTargetViewDescSize);
    m_GraphicsCommandList->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, nullptr);

    // Record some commands 
    m_GraphicsCommandList->ClearRenderTargetView(renderTargetViewHandle, m_ClearColor, 0, nullptr);
}

void DX12Renderer::setRenderState(RenderState* ps)
{
	RenderState_DX12* renderState = static_cast<RenderState_DX12*>(ps);
	m_GraphicsCommandList->SetPipelineState(renderState->GetPipelineState());
}

void DX12Renderer::submit(Mesh* mesh)
{
    m_DrawList.push_back(mesh);
}

void DX12Renderer::frame()
{
	m_GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Adding meshes to to drawing
    for (size_t i = 1; i < m_DrawList.size(); i++)
    {
        Mesh* mesh = m_DrawList[i];

        // Setting the textures
		for (auto& texture : mesh->textures)
		{
			CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(SceneDescHeap->GetGPUDescriptorHandleForHeapStart(), 0, m_CBV_SRV_UAV_Heap_Size);
			m_GraphicsCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TEXTURE, srvHandle);
		}

		// Setting unique render state
        mesh->technique->enable(this);

		// Setting diffuse
		Material_DX12* mat = static_cast<Material_DX12*>(mesh->technique->getMaterial());
		if (mat->constantBuffers.size() != 0)
		{
			CD3DX12_GPU_DESCRIPTOR_HANDLE diffuseHandle(SceneDescHeap->GetGPUDescriptorHandleForHeapStart(),
				mat->constantBuffers.at(DIFFUSE_TINT)->GetLocationInHeap(), m_CBV_SRV_UAV_Heap_Size);
			m_GraphicsCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_DIFFUSE, diffuseHandle);
		}

        // Setting vertex data for vertex pulling
        size_t numberOfVertexBuffers = mesh->geometryBuffers[0].numElements;
        for (size_t n = 0; n < numberOfVertexBuffers; n++)
			m_GraphicsCommandList->SetGraphicsRootShaderResourceView(n, static_cast<VertexBuffer_DX12*>(mesh->geometryBuffers[n].buffer)->getVertexBuffer()->GetGPUVirtualAddress());

        // Setting the translate constant buffer
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(SceneDescHeap->GetGPUDescriptorHandleForHeapStart(),
			static_cast<ConstantBuffer_DX12*>(mesh->txBuffer)->GetLocationInHeap(), m_CBV_SRV_UAV_Heap_Size);
		m_GraphicsCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TRANSLATE, cbvHandle);

        // Drawing triangle mesh
        m_GraphicsCommandList->DrawInstanced(3, 1, 0, 0);
	}

    // Draw all the meshes
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

    if (Device)
    {
        Device->Release();
        Device = nullptr;
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

    if (SceneDescHeap)
    {
        SceneDescHeap->Release();
        SceneDescHeap = nullptr;
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