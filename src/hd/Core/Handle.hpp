#pragma once

namespace hd {

template<typename T, typename Tag, T INVALID_VALUE = static_cast<T>(0)>
struct Handle {
    Handle() : value(INVALID_VALUE) {}

    explicit Handle(const T &value) : value(value) {}

    Handle(const Handle &h) : value(h.value) {}

    Handle &operator=(const Handle &right) {
        this->value = right.value;
        return *this;
    }

    Handle &operator=(const T &right) {
        this->value = right;
        return *this;
    }

    bool operator==(const Handle &right) const {
        return this->value == right.value;
    }

    bool operator!=(const Handle &right) const {
        return this->value != right.value;
    }

    operator bool() const {
        return isValid();
    }

    T& operator->() {
        return this->value;
    }

    const T& operator->() const {
        return this->value;
    }

    bool isValid() const {
        return this->value != INVALID_VALUE;
    }

    template<typename NT>
    NT cast() {
        return static_cast<NT>(this->value);
    }
    template<typename NT>
    NT cast() const {
        return cast<NT>();
    }

    void invalidate() {
        this->value = INVALID_VALUE;
    }

    T value;
};

}
