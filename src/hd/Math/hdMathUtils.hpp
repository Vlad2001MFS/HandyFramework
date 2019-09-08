#pragma once
#include "../Core/hdMacros.hpp"
#include "glm/glm.hpp"

namespace hd {

class MathUtils {
    HD_STATIC_CLASS(MathUtils);
public:
    static bool compareFloat(float a, float b);
    static bool compareDouble(double a, double b);

    static int randomInt32(int min, int max);

    static glm::mat4 ortho2D(float left, float right, float bottom, float top);
};

class AABB {
public:
    AABB();
    AABB(const glm::vec3 &pos, const glm::vec3 &size);

    bool intersectAABB(const AABB &aabb) const;

    void setPosition(const glm::vec3 &pos);
    void setSize(const glm::vec3 &size);
    const glm::vec3 &getPosition() const;
    const glm::vec3 &getSize() const;

private:
    glm::vec3 mPos, mSize;
};

struct RayIntersectAABBInfo {
    RayIntersectAABBInfo();

    glm::vec3 point;
    float dist;
    bool hasIntersection;
};

class Ray {
public:
    Ray();
    Ray(const glm::vec3 &origin, const glm::vec3 &dir);

    RayIntersectAABBInfo intersectAABB(const AABB &aabb) const;

    void setOrigin(const glm::vec3 &origin);
    void setDirection(const glm::vec3 &dir);
    const glm::vec3 &getOrigin() const;
    const glm::vec3 &getDirection() const;

private:
    glm::vec3 mOrigin, mDir;
};

}
