#include "Dx11Render.h"
#include <iostream>
#include <cstdio>
#include <amp.h>
#include <amp_graphics.h>
#include <amp_math.h>
#include <amprt.h>
using namespace Concurrency;
using namespace Concurrency::fast_math;
using namespace Concurrency::graphics;
using namespace Concurrency::graphics::direct3d;
//#include "math/LifeMath.h"
#include "Timer.h"



#include <fstream>
#include <utility>
struct SimpleVertex
{
    float4 Pos;
    float2 TC;
};

Dx11Render::Dx11Render(LifeCore::RenderWindowWin* win, int width, int height)
    :dx_device(nullptr), immediateContext(nullptr), window(win)
{
    HRESULT hr = S_OK;
   
   

    DXGI_SWAP_CHAIN_DESC sw = {0};
    sw.BufferCount = 1;
    sw.BufferDesc.Width = width;
    sw.BufferDesc.Height = height;
    sw.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sw.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sw.BufferDesc.RefreshRate.Numerator = 60;
    sw.BufferDesc.RefreshRate.Denominator = 1;
    sw.OutputWindow = (HWND)window->GetWindowHandle();
    sw.SampleDesc.Count = 1;
    sw.SampleDesc.Quality = 0;
    sw.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sw.Windowed = true;


	D3D_DRIVER_TYPE drivers[]=
	{
		D3D_DRIVER_TYPE_HARDWARE,
	};

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
    };

    int numDrivers = 1;
    D3D_FEATURE_LEVEL selected_features;

    for(int i = 0; i< numDrivers; i++)
    {
#ifdef _DEBUG
        hr = D3D11CreateDevice(0,drivers[i],NULL, D3D11_CREATE_DEVICE_DEBUG,featureLevels,1,D3D11_SDK_VERSION,&dx_device, &selected_features, &immediateContext);
#else
        hr = D3D11CreateDevice(0, drivers[i], NULL, NULL,featureLevels,1,D3D11_SDK_VERSION,&dx_device, &selected_features, &immediateContext);
#endif
        if(SUCCEEDED(hr))
            break;
    }

   

    dx_device->QueryInterface(__uuidof(IDXGIDevice), (void**)(&pDXGIDevice));
    pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)(&pDXGIAdapter));
    pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)(&pFactory));
    pDXGIAdapter->EnumOutputs(0, &pDXGIOutput);

    pFactory->CreateSwapChain(dx_device, &sw, &pSwapChain);


    pFactory->MakeWindowAssociation((HWND)window->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
    if(FAILED(hr))
    {
        printf("Failed to create DirectX device!\n");
        getchar();
        exit(0);
    }






    hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
    hr = dx_device->CreateRenderTargetView(pBuffer, NULL, &pBackBuffer);


    SimpleVertex geometry[] = 
    {
        float4(-1.0f,-1.0f,0.0f,1.0f), float2(0.0f, 1.0f),
        float4(-1.0f,1.0f,0.0f,1.0f), float2(0.0f, 0.0f),

        float4(1.0f,-1.0f,0.0f,1.0f), float2(1.0f, 1.0f),
        float4(1.0f,1.0f,0.0f,1.0f), float2(1.0f, 0.0f),     
    };

    ID3D11Buffer* fsQuadBuffer;
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = geometry;

    hr = dx_device->CreateBuffer( &bd, &InitData, &fsQuadBuffer );

   

    char vs[] = "struct VS_OUTPUT" \
        "{float4 Position : SV_Position;" \
        "float2 Texcoord : TEXCOORD0;};" \
        "void main_VS( in float4 Pos : POSITION, in float2 t : TEXCOORD0, out float4 Position : SV_Position, out float2 texcoord : TEXCOORD0 )" \
        "{Position = Pos; texcoord = t;}";

    char ps[] = "Texture2D diffuseTexture : register(t0);" \
        "SamplerState diffuseSampler : register(s0);" \
        "half4 main_PS( in float4 Pos : SV_Position, in float2 t : TEXCOORD ) : SV_Target" \
        "{float4 data1 = diffuseTexture.Sample(diffuseSampler, t); "\
		"return data1; }";
		//"return lerp(data2, data1, TexW); }";
    /*char ps[] = "Texture2D diffuseTexture : register(t0);" \
        "SamplerState diffuseSampler : register(s0);" \
        "half4 main_PS( in float4 Pos : SV_Position, in float2 t : TEXCOORD ) : SV_Target" \
        "{half4 data = diffuseTexture.Sample(diffuseSampler, t); return float4(t.x, t.y, 0, 1); }";*/

   

    //compile quad vs
    ID3DBlob* vbblob = NULL;
    hr = D3DX11CompileFromMemory(vs, strlen(vs), "system_shader", NULL, NULL, "main_VS", "vs_5_0",0,0,NULL, &vbblob,NULL, NULL);
    hr = dx_device->CreateVertexShader(vbblob->GetBufferPointer(), vbblob->GetBufferSize(), NULL, &vsQuad);


    D3D11_INPUT_ELEMENT_DESC desc[] = 
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    
    UINT numElements = ARRAYSIZE( desc );
    fsQuadLayout = 0;
    hr = dx_device->CreateInputLayout(&desc[0],numElements, vbblob->GetBufferPointer(), vbblob->GetBufferSize(), &fsQuadLayout);


    //compile quad ps
    ID3DBlob* blob = NULL;
    ID3DBlob* blobErr = NULL;
    hr = D3DX11CompileFromMemory(ps, strlen(ps), "PixelShader_quad", NULL, NULL, "main_PS", "ps_5_0",0,0,NULL, &blob,NULL, NULL);
    hr = dx_device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &psQuad);


    D3D11_RASTERIZER_DESC rsDesc;
    rsDesc.AntialiasedLineEnable = false;
    rsDesc.CullMode  = D3D11_CULL_NONE;
    rsDesc.DepthBias = 0;
    rsDesc.DepthBiasClamp = 0.0f;
    rsDesc.SlopeScaledDepthBias = 0.0f;
    rsDesc.DepthClipEnable = TRUE;
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.FrontCounterClockwise = FALSE;
    rsDesc.MultisampleEnable = FALSE;
    rsDesc.ScissorEnable = TRUE;

    ID3D11RasterizerState* rState;

    dx_device->CreateRasterizerState(&rsDesc, &rState);
    immediateContext->RSSetState(rState);


    D3D11_VIEWPORT vp;
    vp.Height = (float)height;
    vp.Width = (float)width;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    immediateContext->RSSetViewports(1, &vp);

    D3D11_RECT sr2;
    sr2.top = 0;
    sr2.left = 0;
    sr2.right = width;
    sr2.bottom = height;
    immediateContext->RSSetScissorRects(1, &sr2);


    
    D3D11_BUFFER_DESC desc1 = {0};
    desc1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc1.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc1.ByteWidth = sizeof(float4);
    desc1.Usage = D3D11_USAGE_DYNAMIC;

    hr = dx_device->CreateBuffer(&desc1, NULL, &constBuf);

    immediateContext->PSSetConstantBuffers(0,1, &constBuf);
    immediateContext->VSSetConstantBuffers(0,1, &constBuf);

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;	

    immediateContext->IASetVertexBuffers(0,1, &fsQuadBuffer, &stride, &offset);

    immediateContext->IASetInputLayout(fsQuadLayout);
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


    immediateContext->VSSetShader(vsQuad, NULL, NULL);
    immediateContext->PSSetShader(psQuad, NULL, NULL);


    ID3D11SamplerState* samplerPoint;
    D3D11_SAMPLER_DESC lsDesc;
    ZeroMemory(&lsDesc, sizeof(D3D11_SAMPLER_DESC));
    lsDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    lsDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    lsDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    lsDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    lsDesc.MaxAnisotropy = 0;
    lsDesc.MinLOD = 0;
    lsDesc.MaxLOD = 0;
    lsDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    dx_device->CreateSamplerState(&lsDesc,&samplerPoint);
    immediateContext->PSSetSamplers(0,1, &samplerPoint);
    immediateContext->OMSetRenderTargets(1, &pBackBuffer, nullptr);
    immediateContext->PSSetShader(psQuad, 0, 0);
    immediateContext->VSSetShader(vsQuad, 0, 0);
}
Dx11Render::~Dx11Render(){

}
Tex2d* Dx11Render::CreateAmpTex(int width, int height){
    D3D11_TEXTURE2D_DESC desc; 
    ZeroMemory(&desc, sizeof(desc)); 
    desc.Height = height; 
    desc.Width = width; 
    desc.MipLevels = 1; 
    desc.ArraySize = 1; 
    desc.Format = DXGI_FORMAT_R32G32B32A32_TYPELESS; 
    desc.SampleDesc.Count = 1; 
    desc.SampleDesc.Quality = 0; 
    desc.Usage = D3D11_USAGE_DEFAULT; 
    desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE; 
    desc.CPUAccessFlags = 0; 
    desc.MiscFlags = 0;

    ID3D11Texture2D *dx_texture = nullptr; 
    HRESULT hr = dx_device->CreateTexture2D( &desc, NULL, &dx_texture );

    ID3D11ShaderResourceView* rv;
   // dx_device->CreateShaderResourceView(dx_texture, NULL, &rv);


    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    dx_device->CreateShaderResourceView(dx_texture, &shaderResourceViewDesc, &rv);

    Concurrency::accelerator_view acc_view = Concurrency::direct3d::create_accelerator_view(dx_device); 
    std::wcout << acc_view.accelerator.description << std::endl;

    // texture with uint data 
    Concurrency::graphics::texture<Concurrency::graphics::direct3d::float4, 2> amp_texture_int = Concurrency::graphics::direct3d::make_texture<Concurrency::graphics::direct3d::float4, 2>( acc_view,   dx_texture);
    Tex2d* tex = new Tex2d();
    tex->srv = rv;
    tex->tex = dx_texture;
    tex->AmpTextureView.reset(new Concurrency::graphics::writeonly_texture_view<Concurrency::graphics::direct3d::float4, 2>(amp_texture_int));
    return tex;
}
void Dx11Render::displayTex(Tex2d* tex){
	ID3D11RenderTargetView* pRTVs[16] = { pBackBuffer,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	immediateContext->OMSetRenderTargets( 8, pRTVs, NULL );
	ID3D11ShaderResourceView* pSRVs[16] = { tex->srv, 0 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    immediateContext->PSSetShaderResources(0,2,pSRVs);
    immediateContext->Draw(4, 0);
}
void Dx11Render::UnbindRtv(){
	ID3D11RenderTargetView* pRTVs[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	ID3D11DepthStencilView* pDSV = NULL;
	immediateContext->OMSetRenderTargets( 8, pRTVs, pDSV );
}
void Dx11Render::UnbindSrv(){
	ID3D11ShaderResourceView* pSRVs[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	immediateContext->VSSetShaderResources( 0, 16, pSRVs );
	immediateContext->PSSetShaderResources( 0, 16, pSRVs );
}
void Dx11Render::present(){
    //present picture on screen
    pSwapChain->Present(0, 0);
	ID3D11ShaderResourceView* pSRVs[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	ID3D11RenderTargetView* pRTVs[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	ID3D11DepthStencilView* pDSV = NULL;
	immediateContext->VSSetShaderResources( 0, 16, pSRVs );
	immediateContext->PSSetShaderResources( 0, 16, pSRVs );
	immediateContext->OMSetRenderTargets( 8, pRTVs, pDSV );

    //ID3D11ShaderResourceView* views[] = {nullptr, nullptr};
    //immediateContext->PSSetShaderResources(0,1, views);
}