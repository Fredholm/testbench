#ifndef TECHNIQUE_DX12_H
#define TECHNIQUE_DX12_H

#include "../Technique.h"

class Material;
class RenderState;
class RenderState_DX12;
class Technique_DX12 : public Technique
{
public:
	Technique_DX12(Material* m, RenderState* r);
	virtual ~Technique_DX12();

	virtual void enable(Renderer* renderer);

private:
	RenderState_DX12* m_RenderState;
};

#endif // !TECHNIQUE_H
