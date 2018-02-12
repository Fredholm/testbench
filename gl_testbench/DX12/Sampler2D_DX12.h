#ifndef SAMPLER2D_DX12_H
#define SAMPLER2D_DX12_H

#include "../Sampler2D.h"

class Sampler2D_DX12 : public Sampler2D
{
public:
    Sampler2D_DX12();
    ~Sampler2D_DX12();

    void setMagFilter(FILTER filter);
    void setMinFilter(FILTER filter);
    void setWrap(WRAPPING s, WRAPPING t);
};


#endif // !SAMPLER2D_DX12_H
