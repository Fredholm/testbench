﻿#ifndef DX12RENDERER_H
#define DX12RENDERER_H

// Include of the abstract Renderer class
#include "../Renderer.h"

#pragma comment(lib,"D3D12.lib")
#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"D3DCompiler.lib")

// DirectX12 Includes 
#include <d3d12.h>
#include <d3dx12.h> // DX12 helper functions and structs
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>

// Common Pointer Object, used once, fuck'em
#include <wrl.h>
#include <shellapi.h>
using Microsoft::WRL::ComPtr;

// SDL Includes and Libs
#include <SDL.h>
#include <SDL_syswm.h> // For SDL_Window HWND retrival
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")

class DX12Renderer : public Renderer
{
private:
    struct Options
    {
        static const D3D12_PRIMITIVE_TOPOLOGY_TYPE Topology     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        static const D3D_FEATURE_LEVEL FeatureLevel             = D3D_FEATURE_LEVEL_11_0;
        static const DXGI_FORMAT Format                         = DXGI_FORMAT_R8G8B8A8_UNORM;   //< 32bits -> 8 bits per channel including alpha
        static const UINT FrameCount                            = 2;
    };

public:
    DX12Renderer();
    virtual ~DX12Renderer();

    Material* makeMaterial(const std::string& name);
    Mesh* makeMesh();
    VertexBuffer* makeVertexBuffer(size_t size, VertexBuffer::DATA_USAGE usage);
    Texture2D* makeTexture2D();
    Sampler2D* makeSampler2D();
    RenderState* makeRenderState();
    std::string getShaderPath();
    std::string getShaderExtension();
    ConstantBuffer* makeConstantBuffer(std::string NAME, unsigned int location);
    Technique* makeTechnique(Material* material, RenderState* renderState);

    int initialize(unsigned int width = 800, unsigned int height = 600);
    void setWinTitle(const char* title);
    int shutdown();

    void setClearColor(float r, float g, float b, float a);
    void clearBuffer(unsigned int);

    // can be partially overriden by a specific Technique.
    void setRenderState(RenderState* ps);

    // \brief Submitting work to the renderer
    void submit(Mesh* mesh);

    // \brief Called each frame 
    // Basicly the update and render functions
    void present();
    void frame();

private:

    ///////////////////////////////////
    // SDL Window Creation and objects
    void initSDLWindow(unsigned int width, unsigned int height);
    SDL_Window*                 m_SDLWindow;
    HWND                        m_WindowHandle;
    HDC                         m_WindowContext;
    ///////////////////////////////////
    
    // DirectX 12 Initilization
    void initDX12(unsigned int width, unsigned int height);
    void loadPipeline(unsigned int width, unsigned int height);
    void loadAssets();

    // DirectX 12 Helper Functions
    void getHardwareAdapter(IDXGIFactory2* factory, IDXGIAdapter1** adapter);
    void waitForTheGPU();
    void moveToNextFrame();

    // Simple Stuff
    std::vector<Mesh*>          m_DrawList;
    float                       m_AspectRatio;
    CD3DX12_VIEWPORT            m_Viewport;
    CD3DX12_RECT                m_ScissorRect;
    float                       m_ClearColor[4];

public:
    // DirectX12 Pipeline
	static ID3D12Device*			   m_Device;
    static IDXGISwapChain3*            m_SwapChain;                                //< IDXGISwapChain3::GetCurrentBackBufferIndex to choose which Render Target View to render to each frame
    static ID3D12Resource*             m_RenderTargets[Options::FrameCount];       //< Contains arrays of data, Enables both the GPU and CPU to read the physical memory   
                                                                            //< \note CreateReservedResource is just virtual, while CreateCommitedResource is both physical and virtual, CreateHeap is still just physical
    // DirectX Command Things 
    // (Work submission was automaticly in DirectX11, 
    //  so this is all new stuff)
    static ID3D12CommandAllocator*     m_CommandAllocator;                         //< Used to back memory for the commands
    static ID3D12CommandQueue*         m_CommandQueue;                             //< FIFO scheduler
    static ID3D12GraphicsCommandList*  m_GraphicsCommandList;                      //< Copied from docs, since I've never used it: Includes APIs for instrumenting the command list execution, and for setting and clearing the pipeline state
    static ID3D12RootSignature*        m_RootSignature;                            //< Used to Link command lists to the resources the shaders need

    // DirectX Resource Binding
    static ID3D12DescriptorHeap*       m_rtDescriptorHeap;                         //< Used as Resource Binding, They completly reworked the RB system, -> CommandList::Set*Root()DescriptorTable blabla
    static ID3D12DescriptorHeap*       m_sceneDescriptorHeap;                         //< Used as Resource Binding, They completly reworked the RB system, -> CommandList::Set*Root()DescriptorTable blabla
    static UINT                        m_RenderTargetViewDescSize;
    static UINT                        m_CBV_SRV_UAV_Heap_Size;

    // DirectX Fence Sync
    static ID3D12Fence*                m_Fence;                                    //< Used to jump between and sync GPU and CPU
    static UINT                        m_FrameIndex;                               //< Current Fence Index
    static HANDLE                      m_FenceEvent;                               //< Current Fence Event
    static UINT64                      m_FenceValue;                               //< Current Fence Value
};

#endif // !DX12RENDERER_H
