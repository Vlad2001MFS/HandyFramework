#pragma once
#include "../System/hdWindow.hpp"
#include <memory>

namespace hd {

class RenderDevice {
public:
    //RenderDevice();
    //RenderDevice();
    //~RenderDevice();

    //void create();
    //void destroy();



private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

}