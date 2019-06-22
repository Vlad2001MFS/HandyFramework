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

    template<typename ...Args2>
    void connect(const auto &func, Args2 &&...args) {
        auto callback = static_cast<std::function<void(Args...)>>(func);
        mFunctions.push_back(std::bind(func, args...));
    }
    
    template<typename ClassType, typename ...Args2>
    void connect(void(ClassType::*func)(Args...), Args2 &&...args) {
        mFunctions.push_back(std::bind(func, args...));
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
