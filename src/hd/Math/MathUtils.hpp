#pragma once
#include "../Core/Common.hpp"
#include <glm/glm.hpp>

namespace hd {

class MathUtils : public StaticClass {
public:
    static int randomInt32(int min, int max);
    static glm::mat4 ortho2D(float left, float right, float bottom, float top);
    static glm::vec2 rotate2D(float vx, float vy, float angle);
    static glm::vec2 rotate2D(const glm::vec2 &v, float angle);
};

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
