#include "hdRenderDevice.hpp"
#ifdef HD_GAPI_D3D11
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#define HD_RELEASE(obj) do { obj->Release(); obj = nullptr; } while (false)

namespace hd {

struct RenderDevice::Impl {
    void create(const Window &window, bool fullscreen, uint32_t msaaSamples) {
        createDeviceAndContext();
        createSwapChain(window, fullscreen, msaaSamples);
        createDefaultRTV();
        createDefaultDSV(window, msaaSamples);
        setRenderTarget(nullptr, nullptr);
        setViewport(0, 0, window.getSizeX(), window.getSizeY());
    }

    void destroy() {
        HD_RELEASE(this->depthStencilView);
        HD_RELEASE(this->depthStencilBuffer);
        HD_RELEASE(this->backBufferRTV);
        HD_RELEASE(this->backBuffer);
        HD_RELEASE(this->swapChain);
        HD_RELEASE(this->dxgiFactory);
        HD_RELEASE(this->dxgiAdapter);
        HD_RELEASE(this->dxgiDevice);
        HD_RELEASE(this->context);
        HD_RELEASE(this->device);
    }

    void createDeviceAndContext() {
        auto requiredFeatureLevel = D3D_FEATURE_LEVEL_11_0;
        D3D_FEATURE_LEVEL supportedFeatureLevel;
#ifdef HD_BUILDMODE_DEBUG
        auto hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, &requiredFeatureLevel, 1, D3D11_SDK_VERSION, &this->device, &supportedFeatureLevel, &this->context);
#else
        auto hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &requiredFeatureLevel, 1, D3D11_SDK_VERSION, &this->device, &supportedFeatureLevel, &this->context);
#endif
        if (FAILED(hr)) {
            HD_LOG_ERROR("Failed to create D3D11Device and D3D11DeviceContext. Error: %d", hr);
        }
        if (supportedFeatureLevel != requiredFeatureLevel) {
            HD_LOG_ERROR("Feature level 11.0 not supported");
        }
    }

    DXGI_SAMPLE_DESC getSampleDesc(DXGI_FORMAT format, uint32_t samples) {
        uint32_t quality;
        auto hr = this->device->CheckMultisampleQualityLevels(format, samples, &quality);
        if (FAILED(hr)) {
            HD_LOG_ERROR("Failed to check MSAA %dx support. Error: %d", samples, hr);
        }
        if (quality <= 0) {
            HD_LOG_ERROR("MSAA %dx not supported", samples);
        }
        DXGI_SAMPLE_DESC sd;
        sd.Count = samples;
        sd.Quality = quality - 1;
        return sd;
    }

    void createSwapChain(const Window &window, bool fullscreen, uint32_t msaaSamples) {
        auto backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        this->sampleDesc = getSampleDesc(backBufferFormat, msaaSamples);

        DXGI_SWAP_CHAIN_DESC sd;
        sd.BufferCount = 1;
        sd.BufferDesc.Width = window.getSizeX();
        sd.BufferDesc.Height = window.getSizeY();
        sd.BufferDesc.Format = backBufferFormat;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = static_cast<HWND>(window.getOSWindowHandle());
        sd.SampleDesc = this->sampleDesc;
        sd.Windowed = !fullscreen;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        auto hr = this->device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&this->dxgiDevice));
        if (FAILED(hr)) {
            HD_LOG_ERROR("Failed to query DXGIDevice. Error: %d", hr);
        }
        hr = this->dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&this->dxgiAdapter));
        if (FAILED(hr)) {
            HD_LOG_ERROR("Failed to get DXGIAdapter. Error: %d", hr);
        }
        hr = this->dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&this->dxgiFactory));
        if (FAILED(hr)) {
            HD_LOG_ERROR("Failed to get DXGIFactory. Error: %d", hr);
        }

        hr = this->dxgiFactory->CreateSwapChain(this->device, &sd, &this->swapChain);
        if (FAILED(hr)) {
            HD_LOG_ERROR("Failed to create D3D11SwapChain. Error: %d", hr);
        }
    }

    void createDefaultRTV() {
        auto hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&this->backBuffer));
        if (FAILED(hr)) {
            HD_LOG_ERROR("Failed to get D3D11Texture2D of back buffer. Error: %d", hr);
        }
        hr = this->device->CreateRenderTargetView(this->backBuffer, nullptr, &this->backBufferRTV);
        if (FAILED(hr)) {
            HD_LOG_ERROR("Failed to create D3D11RenderTargetView for back buffer texture");
        }
    }

    void createDefaultDSV(const Window &window, uint32_t msaaSamples) {
        D3D11_TEXTURE2D_DESC td;
        td.Width = window.getSizeX();
        td.Height = window.getSizeY();
        td.MipLevels = 1;
        td.ArraySize = 1;
        td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        td.SampleDesc = this->sampleDesc;
        td.Usage = D3D11_USAGE_DEFAULT;
        td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        td.CPUAccessFlags = 0;
        td.MiscFlags = 0;

        auto hr = this->device->CreateTexture2D(&td, nullptr, &this->depthStencilBuffer);
        if (FAILED(hr)) {
            HD_LOG_ERROR("Failed to create D3D11Texture2D for depth stencil. Error: %d", hr);
        }
        hr = this->device->CreateDepthStencilView(this->depthStencilBuffer, nullptr, &this->depthStencilView);
        if (FAILED(hr)) {
            HD_LOG_ERROR("Failed to create D3D11DepthStencilView. Error: %d", hr);
        }
    }

    void resizeBuffers(uint32_t w, uint32_t h) {
        HD_RELEASE(this->backBufferRTV);
        HD_RELEASE(this->backBuffer);
        auto hr = this->swapChain->ResizeBuffers(1, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
        if (FAILED(hr)) {
            HD_LOG_ERROR("Failed to resize DXGISwapChain buffers. Error: %d", hr);
        }
        createDefaultRTV();
    }

    void present() {
        auto hr = this->swapChain->Present(0, 0);
        if (FAILED(hr)) {
            HD_LOG_ERROR("DXGISwapChain::Present failed. Error: %d", hr);
        }
    }

    void clearRenderTarget(ID3D11RenderTargetView *rtv, float r, float g, float b, float a) {
        const float color[] = { r, g, b, a };
        if (rtv) {
            this->context->ClearRenderTargetView(rtv, color);
        }
        else {
            this->context->ClearRenderTargetView(this->backBufferRTV, color);
        }
    }
    
    void clearDepthStencil(ID3D11DepthStencilView *dsv, float depth, uint8_t stencil) {
        if (dsv) {
            this->context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
        }
        else {
            this->context->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
        }
    }

    void clearDepth(ID3D11DepthStencilView *dsv, float depth) {
        if (dsv) {
            this->context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, depth, 0);
        }
        else {
            this->context->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, depth, 0);
        }
    }

    void clearStencil(ID3D11DepthStencilView *dsv, uint8_t stencil) {
        if (dsv) {
            this->context->ClearDepthStencilView(dsv, D3D11_CLEAR_STENCIL, 1.0f, stencil);
        }
        else {
            this->context->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_STENCIL, 1.0f, stencil);
        }
    }

    void setRenderTarget(ID3D11RenderTargetView *rtv, ID3D11DepthStencilView *dsv) {
        this->context->OMSetRenderTargets(1, rtv ? &rtv : &this->backBufferRTV, dsv ? dsv : this->depthStencilView);
    }

    void setViewport(int x, int y, int w, int h) {
        D3D11_VIEWPORT vp;
        vp.Width = w;
        vp.Height = h;
        vp.TopLeftX = x;
        vp.TopLeftY = y;
        vp.MaxDepth = 1.0f;
        vp.MinDepth = 0.0f;
        this->context->RSSetViewports(1, &vp);
    }

    ID3D11Device *device;
    IDXGIDevice *dxgiDevice;
    IDXGIAdapter *dxgiAdapter;
    IDXGIFactory *dxgiFactory;
    IDXGISwapChain *swapChain;
    ID3D11DeviceContext *context;
    ID3D11Texture2D *backBuffer;
    ID3D11RenderTargetView *backBufferRTV;
    ID3D11Texture2D *depthStencilBuffer;
    ID3D11DepthStencilView *depthStencilView;
    DXGI_SAMPLE_DESC sampleDesc;
};

RenderDevice::RenderDevice() : impl(std::make_unique<Impl>()) {
}

RenderDevice::RenderDevice(const Window &window, bool fullscreen, uint32_t msaaSamples) : RenderDevice() {
    create(window, fullscreen, msaaSamples);
}

RenderDevice::~RenderDevice() {
    destroy();
}

void RenderDevice::create(const Window &window, bool fullscreen, uint32_t msaaSamples) {
    impl->create(window, fullscreen, msaaSamples);
}

void RenderDevice::destroy() {
    impl->destroy();
}

void RenderDevice::resizeBuffers(uint32_t w, uint32_t h) {
    impl->resizeBuffers(w, h);
}

void RenderDevice::present() {
    impl->present();
}

void RenderDevice::clearRenderTarget(float r, float g, float b, float a) {
    impl->clearRenderTarget(nullptr, r, g, b, a);
}

void RenderDevice::clearDepthStencil(float depth, uint8_t stencil) {
    impl->clearDepthStencil(nullptr, depth, stencil);
}

void RenderDevice::clearDepth(float depth) {
    impl->clearDepth(nullptr, depth);
}

void RenderDevice::clearStencil(uint8_t stencil) {
    impl->clearStencil(nullptr, stencil);
}

void RenderDevice::setViewport(int x, int y, int w, int h) {
    impl->setViewport(x, y, w, h);
}

}

#endif