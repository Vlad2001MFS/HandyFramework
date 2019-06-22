#pragma once
#include "../System/hdWindow.hpp"
#include <memory>

namespace hd {

class RenderDevice {
public:
    RenderDevice();
    RenderDevice(const Window &window, bool fullscreen, uint32_t msaaSamples);
    ~RenderDevice();

    void create(const Window &window, bool fullscreen, uint32_t msaaSamples);
    void destroy();

    void resizeBuffers(uint32_t w, uint32_t h);
    void present();
    void clearRenderTarget(float r, float g, float b, float a);
    void clearDepthStencil(float depth, uint8_t stencil);
    void clearDepth(float depth);
    void clearStencil(uint8_t stencil);
    void setViewport(int x, int y, int w, int h);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

}