#ifndef SAMPLER2D_DX12_H
#define SAMPLER2D_DX12_H

#include "../Sampler2D.h"
#include <d3d12.h>

class Sampler2D_DX12 : public Sampler2D
{
public:
    Sampler2D_DX12();
    ~Sampler2D_DX12();

    void setMagFilter(FILTER filter);
    void setMinFilter(FILTER filter);
    void setWrap(WRAPPING s, WRAPPING t);
	void recreate();

	UINT GetLocationInHeap() { return m_LocationInHeap; }

private:
	D3D12_SAMPLER_DESC m_SamplerDesc;
	UINT m_LocationInHeap;
};


#endif // !SAMPLER2D_DX12_H
