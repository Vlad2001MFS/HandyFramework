#include "hdMathUtils.hpp"
#include <limits>
#include <cmath>
#include <ctime>

namespace hd {

struct Randomizer {
    Randomizer() {
        std::srand(static_cast<uint32_t>(std::clock()));
    }
} _gRandomizer;

bool MathUtils::compareFloat(float a, float b) {
    return std::abs(a - b) < std::numeric_limits<float>::epsilon();
}

bool MathUtils::compareDouble(double a, double b) {
    return std::abs(a - b) < std::numeric_limits<double>::epsilon();
}

int MathUtils::randomInt32(int min, int max) {
    return std::rand() % max + min;
}

AABB::AABB(const glm::vec3 &pos, const glm::vec3 &size) : mPos(pos), mSize(size) {
}

bool AABB::intersectAABB(const AABB &aabb) const {
    glm::vec3 obj1Min = getPosition() - getSize();
    glm::vec3 obj1Max = getPosition() + getSize();
    glm::vec3 obj2Min = aabb.getPosition() - aabb.getSize();
    glm::vec3 obj2Max = aabb.getPosition() + aabb.getSize();
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

const glm::vec3 &AABB::getPosition() const {
    return mPos;
}

const glm::vec3 &AABB::getSize() const {
    return mSize;
}

Ray::Ray(const glm::vec3 &origin, const glm::vec3 &dir) : mOrigin(origin), mDir(dir) {
}

RayIntersectAABBInfo Ray::intersectAABB(const AABB &aabb) const {
    glm::vec3 aabbMin = aabb.getPosition() - aabb.getSize();
    glm::vec3 aabbMax = aabb.getPosition() + aabb.getSize();
    glm::vec3 tMin = (aabbMin - getOrigin()) / getDirection();
    glm::vec3 tMax = (aabbMax - getOrigin()) / getDirection();
    glm::vec3 t1 = glm::min(tMin, tMax);
    glm::vec3 t2 = glm::max(tMin, tMax);
    float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
    float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);
    glm::vec3 point1 = getOrigin() + getDirection()*tNear;
    glm::vec3 point2 = getOrigin() + getDirection()*tFar;

    RayIntersectAABBInfo info;
    info.point = tNear > 0 ? point1 : point2;
    info.dist = tNear < tFar ? tNear : INFINITY;
    info.hasIntersection = tNear < tFar ? true : false;
    return info;
}

const glm::vec3 &Ray::getOrigin() const {
    return mOrigin;
}

const glm::vec3 &Ray::getDirection() const {
    return mDir;
}

}
