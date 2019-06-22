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

/*glm::vec2 MathUtils::rotateVertex(const glm::vec2 &v, float angle) {
    float s = glm::sin(angle), c = glm::cos(angle);
    return glm::vec2(
        v.x*c - v.y*s,
        v.x*s + v.y*c
    );
}

float MathUtils::getSignOfHalfPlane(const glm::vec2 &pointOnLine1, const glm::vec2 &pointOnLine2, const glm::vec2 &point) {
    return (pointOnLine1.x - point.x)*(pointOnLine2.y - point.y) - (pointOnLine1.y - point.y)*(pointOnLine2.x - point.x);
}

bool MathUtils::isPointOnRect(const glm::vec2 &point, const glm::vec2 *rectVertices, const glm::vec2 &rectCenter) {
    glm::vec2 l[4][2] = {
        { rectVertices[0], rectVertices[1] },
        { rectVertices[1], rectVertices[2] },
        { rectVertices[2], rectVertices[3] },
        { rectVertices[3], rectVertices[0] }
    };

    for (int i = 0; i < 4; i++) {
        int op = static_cast<int>(glm::round(glm::clamp(getSignOfHalfPlane(l[i][0], l[i][1], point), -1.0f, 1.0f)));
        int oc = static_cast<int>(glm::round(glm::clamp(getSignOfHalfPlane(l[i][0], l[i][1], rectCenter), -1.0f, 1.0f)));
        bool flag1 = op < 0 && oc < 0;
        bool flag2 = op > 0 && oc > 0;
        bool flag3 = op == 0;
        bool flag = flag1 || flag2 || flag3;
        if (!flag) {
            return false;
        }
    }
    return true;
}

bool MathUtils::isPointOnRect(const glm::ivec2 &point, const glm::ivec2 &rcPos, const glm::ivec2 &rcSize) {
    return point.x >= rcPos.x && point.x < (rcPos.x + rcSize.x) && point.y >= rcPos.y && point.y < (rcPos.y + rcSize.y);
}

bool MathUtils::isPointOnRect(const glm::vec2 &point, const glm::vec2 &rcPos, const glm::vec2 &rcSize) {
    return point.x >= rcPos.x && point.x < (rcPos.x + rcSize.x) && point.y >= rcPos.y && point.y < (rcPos.y + rcSize.y);
}

bool MathUtils::isPointOnCircle(const glm::ivec2 &point, const glm::ivec2 &circlePos, int circleRadius) {
    glm::ivec2 d = circlePos - point;
    float dist = glm::sqrt(static_cast<float>(d.x*d.x + d.y*d.y));
    return dist < circleRadius;
}

bool MathUtils::isPointOnCircle(const glm::vec2 &point, const glm::vec2 &circlePos, float circleRadius) {
    glm::vec2 d = circlePos - point;
    float dist = glm::sqrt(static_cast<float>(d.x*d.x + d.y*d.y));
    return dist < circleRadius;
}

bool MathUtils::isRectIntersectRect(const glm::ivec2 &rc1Pos, const glm::ivec2 &rc1Size, const glm::ivec2 &rc2Pos, const glm::ivec2 &rc2Size) {
    glm::ivec2 a1(rc2Pos.x, rc2Pos.y); // left up
    glm::ivec2 a2(rc2Pos.x + rc2Size.x, rc2Pos.y); // right up
    glm::ivec2 a3(rc2Pos.x, rc2Pos.y + rc2Size.y); // left down
    glm::ivec2 a4(rc2Pos.x + rc2Size.x, rc2Pos.y + rc2Size.y); // right down
    glm::ivec2 a5(rc2Pos.x + rc2Size.x / 2, rc2Pos.y + rc2Size.y / 2); // center
    bool flag1 = isPointOnRect(a1, rc1Pos, rc1Size) || isPointOnRect(a2, rc1Pos, rc1Size) || isPointOnRect(a3, rc1Pos, rc1Size) ||
                 isPointOnRect(a4, rc1Pos, rc1Size) || isPointOnRect(a5, rc1Pos, rc1Size);

    glm::ivec2 b1(rc1Pos.x, rc1Pos.y); // left up
    glm::ivec2 b2(rc1Pos.x + rc1Size.x, rc1Pos.y); // right up
    glm::ivec2 b3(rc1Pos.x, rc1Pos.y + rc1Size.y); // left down
    glm::ivec2 b4(rc1Pos.x + rc1Size.x, rc1Pos.y + rc1Size.y); // right down
    glm::ivec2 b5(rc1Pos.x + rc1Size.x / 2, rc1Pos.y + rc1Size.y / 2); // center
    bool flag2 = isPointOnRect(b1, rc2Pos, rc2Size) || isPointOnRect(b2, rc2Pos, rc2Size) || isPointOnRect(b3, rc2Pos, rc2Size) ||
                 isPointOnRect(b4, rc2Pos, rc2Size) || isPointOnRect(b5, rc2Pos, rc2Size);
    
    return flag1 || flag2;
}

bool MathUtils::isRectIntersectRect(const glm::vec2 &rc1Pos, const glm::vec2 &rc1Size, const glm::vec2 &rc2Pos, const glm::vec2 &rc2Size) {
    glm::vec2 a1(rc2Pos.x, rc2Pos.y); // left up
    glm::vec2 a2(rc2Pos.x + rc2Size.x, rc2Pos.y); // right up
    glm::vec2 a3(rc2Pos.x, rc2Pos.y + rc2Size.y); // left down
    glm::vec2 a4(rc2Pos.x + rc2Size.x, rc2Pos.y + rc2Size.y); // right down
    glm::vec2 a5(rc2Pos.x + rc2Size.x / 2, rc2Pos.y + rc2Size.y / 2); // center
    bool flag1 = isPointOnRect(a1, rc1Pos, rc1Size) || isPointOnRect(a2, rc1Pos, rc1Size) || isPointOnRect(a3, rc1Pos, rc1Size) ||
                 isPointOnRect(a4, rc1Pos, rc1Size) || isPointOnRect(a5, rc1Pos, rc1Size);

    glm::vec2 b1(rc1Pos.x, rc1Pos.y); // left up
    glm::vec2 b2(rc1Pos.x + rc1Size.x, rc1Pos.y); // right up
    glm::vec2 b3(rc1Pos.x, rc1Pos.y + rc1Size.y); // left down
    glm::vec2 b4(rc1Pos.x + rc1Size.x, rc1Pos.y + rc1Size.y); // right down
    glm::vec2 b5(rc1Pos.x + rc1Size.x / 2, rc1Pos.y + rc1Size.y / 2); // center
    bool flag2 = isPointOnRect(b1, rc2Pos, rc2Size) || isPointOnRect(b2, rc2Pos, rc2Size) || isPointOnRect(b3, rc2Pos, rc2Size) ||
                 isPointOnRect(b4, rc2Pos, rc2Size) || isPointOnRect(b5, rc2Pos, rc2Size);
    
    return flag1 || flag2;
}

bool MathUtils::isRectIntersectCircle(const glm::ivec2 &rcPos, const glm::ivec2 &rcSize, const glm::ivec2 &circlePos, int circleRadius) {
    float r = circleRadius;
    glm::vec2 circle = circlePos;
    glm::vec2 pos = rcPos, size = rcSize;
    glm::vec2 center = pos + size / 2.0f;
    glm::vec2 nearestPoint = circle + glm::normalize(center - circle)*r;
    return hd::MathUtils::isPointOnRect(nearestPoint, pos, size);
}

bool MathUtils::isRectIntersectCircle(const glm::vec2 &rcPos, const glm::vec2 &rcSize, const glm::vec2 &circlePos, float circleRadius) {
    glm::vec2 rcCenter = rcPos + rcSize / 2.0f;
    glm::vec2 nearestPoint = circlePos + glm::normalize(rcCenter - circlePos)*circleRadius;
    return hd::MathUtils::isPointOnRect(nearestPoint, rcPos, rcSize);
}*/

}
