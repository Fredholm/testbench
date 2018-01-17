#ifndef DX12RENDERER_H
#define DX12RENDERER_H

#include "Renderer.h"

// DirectX12 Includes 
#include <d3d12.h>

// SDL Includes and Libs
#include <SDL.h>
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")

class DX12Renderer : public Renderer
{
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

    // submit work (to render) to the renderer.
    void submit(Mesh* mesh);
    void frame();

private:

    // SDL Window Creation and objects
    void initSDLWindown(unsigned int width, unsigned int height);
    SDL_Window* window;
    SDL_GLContext context;

    // DX12 Pipeline

    // DX12 Objects
};


#endif // !DX12RENDERER_H
