#pragma once
#include "../Core/hdCommon.hpp"
//#include "glm/glm.hpp"

namespace hd {

class MathUtils {
    HD_STATIC_CLASS(MathUtils)
public:
    static bool compareFloat(float a, float b);
    static bool compareDouble(double a, double b);

    static int randomInt32(int min, int max);

    /*static glm::vec2 rotateVertex(const glm::vec2 &v, float angle);
    static float getSignOfHalfPlane(const glm::vec2 &pointOnLine1, const glm::vec2 &pointOnLine2, const glm::vec2 &point);
    static bool isPointOnRect(const glm::vec2 &point, const glm::vec2 *rectVertices, const glm::vec2 &rectCenter);
    static bool isPointOnRect(const glm::ivec2 &point, const glm::ivec2 &rcPos, const glm::ivec2 &rcSize);
    static bool isPointOnRect(const glm::vec2 &point, const glm::vec2 &rcPos, const glm::vec2 &rcSize);
    static bool isPointOnCircle(const glm::ivec2 &point, const glm::ivec2 &circlePos, int circleRadius);
    static bool isPointOnCircle(const glm::vec2 &point, const glm::vec2 &circlePos, float circleRadius);
    static bool isRectIntersectRect(const glm::ivec2 &rc1Pos, const glm::ivec2 &rc1Size, const glm::ivec2 &rc2Pos, const glm::ivec2 &rc2Size);
    static bool isRectIntersectRect(const glm::vec2 &rc1Pos, const glm::vec2 &rc1Size, const glm::vec2 &rc2Pos, const glm::vec2 &rc2Size);
    static bool isRectIntersectCircle(const glm::ivec2 &rcPos, const glm::ivec2 &rcSize, const glm::ivec2 &circlePos, int circleRadius);
    static bool isRectIntersectCircle(const glm::vec2 &rcPos, const glm::vec2 &rcSize, const glm::vec2 &circlePos, float circleRadius);*/
};

}
