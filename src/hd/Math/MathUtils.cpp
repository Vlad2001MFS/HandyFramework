#include "MathUtils.hpp"
#include <cmath>
#include <ctime>

namespace hd {

class Random : public Singleton<Random> {
public:
    Random() {
        srand(static_cast<uint32_t>(std::clock()));
    }

    int next() {
        return rand();
    }
};

int MathUtils::randomInt32(int min, int max) {
    return Random::get().next() % max + min;
}

glm::mat4 MathUtils::ortho2D(float left, float right, float bottom, float top) {
    return glm::mat4(
         2.0f / (right - left),            0.0f,                            0.0f, 0.0f,
         0.0f,                            -2.0f / (bottom - top),           0.0f, 0.0f,
         0.0f,                             0.0f,                            1.0f, 0.0f,
        -(right + left) / (right - left),  (bottom + top) / (bottom - top), 0.0f, 1.0f
    );
}

AABB::AABB() : pos(0, 0, 0), size(0, 0, 0) {
}

AABB::AABB(const glm::vec3 &pos, const glm::vec3 &size) : pos(pos), size(size) {
}

bool AABB::intersectAABB(const AABB &aabb) const {
    glm::vec3 obj1Min = pos - size;
    glm::vec3 obj1Max = pos + size;
    glm::vec3 obj2Min = aabb.pos - aabb.size;
    glm::vec3 obj2Max = aabb.pos + aabb.size;
    if (obj1Max.x < obj2Min.x || obj1Min.x > obj2Max.x) {
        return false;
    }
    if (obj1Max.y < obj2Min.y || obj1Min.y > obj2Max.y) {
        return false;
    }
    if (obj1Max.z < obj2Min.z || obj1Min.z > obj2Max.z) {
        return false;
    }
    return true;
}

RayIntersectAABBInfo::RayIntersectAABBInfo() : point(0, 0, 0) {
    this->dist = INFINITY;
    this->hasIntersection = false;
}

Ray::Ray() : origin(0, 0, 0), dir(0, 0, 0) {
}

Ray::Ray(const glm::vec3 &origin, const glm::vec3 &dir) : origin(origin), dir(dir) {
}

RayIntersectAABBInfo Ray::intersectAABB(const AABB &aabb) const {
    glm::vec3 aabbMin = aabb.pos - aabb.size;
    glm::vec3 aabbMax = aabb.pos + aabb.size;
    glm::vec3 tMin = (aabbMin - origin) / dir;
    glm::vec3 tMax = (aabbMax - origin) / dir;
    glm::vec3 t1 = glm::min(tMin, tMax);
    glm::vec3 t2 = glm::max(tMin, tMax);
    float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
    float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);
    glm::vec3 point1 = origin + dir*tNear;
    glm::vec3 point2 = origin + dir*tFar;

    RayIntersectAABBInfo info;
    info.point = tNear > 0 ? point1 : point2;
    info.dist = tNear < tFar ? tNear : INFINITY;
    info.hasIntersection = tNear < tFar ? true : false;
    return info;
}

}
