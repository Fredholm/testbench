#include "Technique_DX12.h"
#include "DX12Renderer.h"
#include "RenderState_DX12.h"
#include "Material_DX12.h"

Technique_DX12::Technique_DX12(Material* m, RenderState* r)
: Technique(m, r)
{
	m_RenderState = static_cast<RenderState_DX12*>(r);
	m_RenderState->setVS(static_cast<Material_DX12*>(m)->getBlob(Material_DX12::ShaderType::VS));
	m_RenderState->setPS(static_cast<Material_DX12*>(m)->getBlob(Material_DX12::ShaderType::PS));
	m_RenderState->set();
}

Technique_DX12::~Technique_DX12() { }

void Technique_DX12::enable(Renderer* renderer)
{
	renderer->setRenderState(m_RenderState);
}
