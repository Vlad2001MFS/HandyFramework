#pragma once
#include "hdCommon.hpp"
#include <vector>
#include <functional>

namespace  hd {

template<typename ...Args>
class Delegate {
    HD_NONCOPYABLE_CLASS(Delegate)
    using FuncSig = void(Args...);
public:
    Delegate() {}

    void connect(const std::function<FuncSig> &f) {
        mFunctions.push_back(f);
    }

    void operator()(Args &&...args) const {
        for (const auto &func : mFunctions) {
            func(args...);
        }
    }

private:
    std::vector<std::function<FuncSig>> mFunctions;
};

}
