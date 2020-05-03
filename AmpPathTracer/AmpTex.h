#ifndef AmpTex_h__
#define AmpTex_h__

#include <d3d11.h>
#include <D3DX11.h>
#include "RenderWindow.h"
#include <amp.h>
#include <amp_graphics.h>
#include <amp_math.h>
#include <amprt.h>
#include <memory>

struct Tex2d{
    std::unique_ptr<Concurrency::graphics::writeonly_texture_view<Concurrency::graphics::direct3d::float4, 2>> AmpTextureView;
    ID3D11Texture2D* tex;
    ID3D11ShaderResourceView* srv;
    Tex2d()
    {

    }
};
#endif // AmpTex_h__
