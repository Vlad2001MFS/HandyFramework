#pragma once
#include "../Core/Common.hpp"
#include <glm/glm.hpp>
#include <limits>
#include <cmath>

namespace hd {

class MathUtils : public StaticClass {
public:
    template<typename T>
    static bool isNearlyEqual(T a, T b);

    template<typename T>
    static bool isNearlyEqual(T a, T b, int epsilonFactor);

    template<typename T, glm::length_t S, glm::qualifier P>
    static bool isNearlyEqual(const glm::vec<S, T, P> &a, const glm::vec<S, T, P> &b);

    template<typename T, glm::length_t S, glm::qualifier P>
    static bool isNearlyEqual(const glm::vec<S, T, P> &a, const glm::vec<S, T, P> &b, int epsilonFactor);

    static int randomInt32(int min, int max);
    static glm::mat4 ortho2D(float left, float right, float bottom, float top);
    static glm::vec2 rotate2D(float vx, float vy, float angle);
    static glm::vec2 rotate2D(const glm::vec2 &v, float angle);
};

template<typename T>
bool MathUtils::isNearlyEqual(T a, T b) {
    return std::nextafter(a, std::numeric_limits<T>::lowest()) <= b && std::nextafter(a, std::numeric_limits<T>::max()) >= b;
}

template<typename T>
bool MathUtils::isNearlyEqual(T a, T b, int epsilonFactor) {
    T minA = a - (a - std::nextafter(a, std::numeric_limits<T>::lowest()))*epsilonFactor;
    T maxA = a + (std::nextafter(a, std::numeric_limits<T>::max()) - a)*epsilonFactor;
    return minA <= b && maxA >= b;
}

template<typename T, glm::length_t S, glm::qualifier P>
bool MathUtils::isNearlyEqual(const glm::vec<S, T, P> &a, const glm::vec<S, T, P> &b) {
    for (glm::length_t i = 0; i < S; i++) {
        if (!isNearlyEqual(a[i], b[i])) {
            return false;
        }
    }
    return true;
}

template<typename T, glm::length_t S, glm::qualifier P>
bool MathUtils::isNearlyEqual(const glm::vec<S, T, P> &a, const glm::vec<S, T, P> &b, int epsilonFactor) {
    for (glm::length_t i = 0; i < S; i++) {
        if (!isNearlyEqual(a[i], b[i], epsilonFactor)) {
            return false;
        }
    }
    return true;
}

struct AABB {
    AABB();
    AABB(const glm::vec3 &pos, const glm::vec3 &size);

    bool intersectAABB(const AABB &aabb) const;

    glm::vec3 pos, size;
};

struct RayIntersectAABBInfo {
    RayIntersectAABBInfo();

    glm::vec3 point;
    float dist;
    bool hasIntersection;
};

struct Ray {
    Ray();
    Ray(const glm::vec3 &origin, const glm::vec3 &dir);

    RayIntersectAABBInfo intersectAABB(const AABB &aabb) const;

    glm::vec3 origin, dir;
};

}
