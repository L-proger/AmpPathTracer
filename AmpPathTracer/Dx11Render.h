#ifndef Dx11Render_h__
#define Dx11Render_h__

#include <d3d11.h>
#include <D3DX11.h>
#include "RenderWindow.h"
#include <amp.h>
#include <amp_graphics.h>
#include <amp_math.h>
#include <amprt.h>
#include <memory>

#include "AmpTex.h"

class Dx11Render{
public:
    Dx11Render(LifeCore::RenderWindowWin* win, int width, int height);
    ~Dx11Render();
    void displayTex(Tex2d* tex);
    void present();
    Tex2d* CreateAmpTex(int width, int height);
	void UnbindSrv();
	void UnbindRtv();
private:
    LifeCore::RenderWindowWin* window;
    ID3D11Device* dx_device; 
    ID3D11DeviceContext* immediateContext;
    ID3D11VertexShader* vsQuad;
    ID3D11PixelShader* psQuad;
    ID3D11InputLayout* fsQuadLayout;
    IDXGIDevice* pDXGIDevice;
    IDXGIAdapter* pDXGIAdapter;
	ID3D11Buffer* constBuf;
    IDXGIFactory* pFactory;
    IDXGIOutput* pDXGIOutput;
    IDXGISwapChain* pSwapChain;
    ID3D11Texture2D* pBuffer;
    ID3D11RenderTargetView* pBackBuffer;
};
#endif // Dx11Render_h__
