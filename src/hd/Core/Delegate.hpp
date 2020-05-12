#pragma once
#include <functional>
#include <vector>

namespace hd {

template<typename... Args>
class Delegate {
    using FuncSig = void(Args...);

public:
    Delegate &operator+=(const std::function<FuncSig> &rhs) {
        mFuncs.push_back(rhs);
        return *this;
    }

    void operator()(Args &&...args) const {
        for (const auto &func : mFuncs) {
            func(args...);
        }
    }

private:
    std::vector<std::function<FuncSig>> mFuncs;
};

}
