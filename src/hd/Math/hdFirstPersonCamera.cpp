#include "hdFirstPersonCamera.hpp"
#include "../../3rd/include/glm/ext.hpp"

namespace hd {

glm::mat4 matFromEuler(const glm::vec3 &rot) {
    return glm::eulerAngleX(rot.x)*glm::eulerAngleY(rot.y)*glm::eulerAngleZ(rot.z);
}

FirstPersonCamera::FirstPersonCamera() : mPos(0, 0, 0), mRot(0, 0, 0), mViewMat(1.0f) {
    mMinVertical = -glm::radians(89.9f);
    mMaxVertical = glm::radians(89.9f);
}

FirstPersonCamera::~FirstPersonCamera() {
}

void FirstPersonCamera::translate(float x, float y, float z) {
    translate(glm::vec3(x, y, z));
}

void FirstPersonCamera::translate(const glm::vec3 &offset) {
    glm::mat4 rotMat = glm::eulerAngleX(mRot.x)*glm::eulerAngleY(mRot.y)*glm::eulerAngleZ(mRot.z);
    glm::vec4 move = glm::transpose(rotMat)*glm::vec4(offset, 1.0f);
    setPosition(mPos + glm::vec3(move));
}

void FirstPersonCamera::rotate(float x, float y, float z) {
    rotate(glm::vec3(x, y, z));
}

void FirstPersonCamera::rotate(const glm::vec3 &angles) {
    setRotation(mRot + angles);
}

void FirstPersonCamera::setPosition(float x, float y, float z) {
    mPos.x = x;
    mPos.y = y;
    mPos.z = z;
}

void FirstPersonCamera::setPosition(const glm::vec3 & pos) {
	setPosition(pos.x, pos.y, pos.z);
}

void FirstPersonCamera::setRotation(float x, float y, float z) {
    mRot.x = glm::clamp(x, mMinVertical, mMaxVertical);
    if (glm::abs(y) > glm::two_pi<float>()) {
        float sign = y / glm::abs(y);
        mRot.y = (glm::abs(y) - glm::two_pi<float>())*sign;
    }
    else {
        mRot.y = y;
    }
    if (glm::abs(z) > glm::two_pi<float>()) {
        float sign = z / glm::abs(z);
        mRot.z = (glm::abs(z) - glm::two_pi<float>())*sign;
    }
    else {
        mRot.z = z;
    }
}

void FirstPersonCamera::setRotation(const glm::vec3 & rot) {
    setRotation(rot.x, rot.y, rot.z);
}

const glm::vec3 &FirstPersonCamera::getPosition() const {
	return mPos;
}

const glm::vec3 &FirstPersonCamera::getRotation() const {
	return mRot;
}

glm::vec3 FirstPersonCamera::getDirectionLH() const {
    glm::mat4 rotMat = glm::eulerAngleX(mRot.x)*glm::eulerAngleY(mRot.y)*glm::eulerAngleZ(mRot.z);
    return glm::vec3(glm::transpose(rotMat)*glm::vec4(0, 0, 1, 0));
}

glm::vec3 FirstPersonCamera::getDirectionRH() const {
    glm::mat4 rotMat = glm::eulerAngleX(mRot.x)*glm::eulerAngleY(mRot.y)*glm::eulerAngleZ(mRot.z);
    return glm::vec3(rotMat*glm::vec4(0, 0, -1, 0));
}

glm::mat4 FirstPersonCamera::getViewMatrixLH() const {
    glm::vec3 eye = mPos;
    glm::vec3 center = eye + getDirectionLH();
    glm::vec3 up = glm::vec3(0, 1, 0);
    return glm::lookAtLH(eye, center, up);
}

glm::mat4 FirstPersonCamera::getViewMatrixRH() const {
    glm::vec3 eye = mPos;
    glm::vec3 center = eye + getDirectionRH();
    glm::vec3 up = glm::vec3(0, 1, 0);
    return glm::lookAtRH(eye, center, up);
}

}