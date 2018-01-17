#include "DX12Renderer.h"

DX12Renderer::DX12Renderer() { }

DX12Renderer::~DX12Renderer() { }

Mesh * DX12Renderer::makeMesh()
{
    //  return new Mesh_DX12();
    return nullptr;
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
    //  return new ConstantBuffer_DX12(NAME, location);
    return nullptr;
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
    //  return new VertexBuffer_DX12(size, usage);
    return nullptr;
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
    initSDLWindown(width, height);


    return 0;
}

void DX12Renderer::initSDLWindown(unsigned int width, unsigned int height)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "%s", SDL_GetError());
        exit(-1);
    }
    // Request an OpenGL 4.5 context (should be core)
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    // Also request a depth buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow("DirectX12", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
    context = SDL_GL_CreateContext(window);

    SDL_GL_MakeCurrent(window, context);
}

void DX12Renderer::setWinTitle(const char * title)
{
    SDL_SetWindowTitle(this->window, title);
}

void DX12Renderer::present()
{
}

int DX12Renderer::shutdown()
{
    return 0;
}

void DX12Renderer::setClearColor(float r, float g, float b, float a)
{
}

void DX12Renderer::clearBuffer(unsigned int)
{
}

void DX12Renderer::setRenderState(RenderState * ps)
{
}

void DX12Renderer::submit(Mesh * mesh)
{
}

void DX12Renderer::frame()
{
}
