#pragma once
#include "../Core/hdCommon.hpp"
#include "../../3rd/include/glm/glm.hpp"

namespace hd {

class MathUtils {
    HD_STATIC_CLASS(MathUtils)
public:
    static bool compareFloat(float a, float b);
    static bool compareDouble(double a, double b);

    static int randomInt32(int min, int max);
};

class AABB {
public:
    AABB(const glm::vec3 &pos, const glm::vec3 &size);

    bool intersectAABB(const AABB &aabb) const;

    const glm::vec3 &getPosition() const;
    const glm::vec3 &getSize() const;

private:
    glm::vec3 mPos, mSize;
};

struct RayIntersectAABBInfo {
    glm::vec3 point;
    float dist;
};

class Ray {
public:
    Ray(const glm::vec3 &origin, const glm::vec3 &dir);

    RayIntersectAABBInfo intersectAABB(const AABB &aabb) const;

    const glm::vec3 &getOrigin() const;
    const glm::vec3 &getDirection() const;

private:
    glm::vec3 mOrigin, mDir;
};

}
