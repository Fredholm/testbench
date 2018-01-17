#ifndef DX12RENDERER_H
#define DX12RENDERER_H

#include "Renderer.h"

// DirectX12 Includes 
#include <d3d12.h>
#include <d3dx12.h> // DX12 helper functions and structs
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

// SDL Includes and Libs
#include <SDL.h>
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")

class DX12Renderer : public Renderer
{
private:
    static const int AmountOfFrames = 2;

public:
    DX12Renderer();
    ~DX12Renderer();

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
    void present();
    int shutdown();

    void setClearColor(float r, float g, float b, float a);
    void clearBuffer(unsigned int);

    // can be partially overriden by a specific Technique.
    void setRenderState(RenderState* ps);

    // \brief Submitting work to the renderer
    void submit(Mesh* mesh);

    // \brief Called each frame 
    // Basicly the render function
    void frame();

private:

    ///////////////////////////////////
    // SDL Window Creation and objects
    void initSDLWindown(unsigned int width, unsigned int height);
    SDL_Window*                 window;
    SDL_GLContext               context;
    ///////////////////////////////////

    CD3DX12_VIEWPORT            m_Viewport;
    CD3DX12_RECT                m_ScissorRect;

    // DirectX12 Pipeline
    IDXGISwapChain3*            m_SwapChain;                        //< IDXGISwapChain3::GetCurrentBackBufferIndex to choose which Render Target View to render to each frame
    ID3D12Device*               m_Device;                           //< Used for a bunch of things, there's no DeviceContext anymore either, Ex Func: CreateCommited/Reserved Resource
    ID3D12Resource*             m_RenderTargets[AmountOfFrames];    //< Contains arrays of data, Enables both the GPU and CPU to read the physical memory   
                                                                    //< \note CreateReservedResource is just virtual, while CreateCommitedResource is both physical and virtual, CreateHeap is still just physical
    // DirectX Command Things 
    // (Work submission was automaticly in DirectX11, 
    //  so this is all new stuff)
    ID3D12CommandAllocator*     m_CommandAllocator;                 //< Used to back memory for the commands
    ID3D12CommandQueue*         m_CommandQueue;                     //< FIFO scheduler
    ID3D12CommandList*          m_CommandList;                      //< (Similar to deffered context in D11), Records commands to be used later
    ID3D12RootSignature*        m_RootSignature;                    //< Used to Link command lists to the resources the shaders need

    // DirectX Other Things
    ID3D12DescriptorHeap*       m_DescriptorHeap;                   //< Used for Resource Binding, They completly reworked the RB system, -> CommandList::Set*Root()DescriptorTable blabla
    ID3D12PipelineState*        m_PipelineState;                    //< Used to identify and use different stuff (VS, PS, HS, DS, GS, CS, OM, RS, IA)
    ID3D12GraphicsCommandList*  m_GraphicsCommandList;              //< Copied from docs, since I've never used it: Includes APIs for instrumenting the command list execution, and for setting and clearing the pipeline state
    
    // DirectX Triangles Things
    ID3D12Resource*             m_VertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW    m_VertexBufferView;

    // DirectX Fence Sync
    ID3D12Fence*                m_Fence;                            //< Used to jump between and sync GPU and CPU
    UINT                        m_FrameIndex;                       //< Current Fence Index
    HANDLE                      m_FenceEvent;                       //< Current Fence Event
    UINT64                      m_FenceValue;                       //< Current Fence Value
};


#endif // !DX12RENDERER_H
