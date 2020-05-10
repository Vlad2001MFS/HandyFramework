#include "FirstPersonCamera.hpp"
#include <glm/ext.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace hd {

FirstPersonCamera::FirstPersonCamera() : mIsRHMode(false), mPos(0, 0, 0), mRot(0, 0, 0), mDir(0, 0, 0), mViewMat(1.0f), mProjMat(1.0f),
        mFov(glm::radians(45.0f)), mAspectRatio(1.0f), mNearClip(0.1f), mFarClip(100.0f), mIsDirDirty(true), mIsViewDirty(true), mIsProjDirty(true) {
    mMinVertical = -glm::radians(89.9f);
    mMaxVertical = glm::radians(89.9f);
    setPosition(0, 0, 0);
    setRotation(0, 0, 0);
    setRHMode(false);
}

void FirstPersonCamera::update(float dt) {
    if (mIsDirDirty) {
        mIsDirDirty = false;
        glm::mat4 rotMat = glm::eulerAngleX(mRot.x)*glm::eulerAngleY(mRot.y)*glm::eulerAngleZ(mRot.z);
        if (isRHMode()) {
            mDir = glm::vec3(rotMat*glm::vec4(0, 0, -1, 0));
        }
        else {
            mDir = glm::vec3(glm::transpose(rotMat)*glm::vec4(0, 0, 1, 0));
        }
    }

    if (mIsViewDirty) {
        mIsViewDirty = false;
        glm::vec3 eye = mPos;
        glm::vec3 up = glm::vec3(0, 1, 0);
        if (isRHMode()) {
            mViewMat = glm::lookAtRH(eye, eye + getDirection(), up);
        }
        else {
            mViewMat = glm::lookAtLH(eye, eye + getDirection(), up);
        }
    }

    if (mIsProjDirty) {
        mIsProjDirty = false;
        if (isRHMode()) {
            mProjMat = glm::perspectiveRH(mFov, mAspectRatio, mNearClip, mFarClip);
        }
        else {
            mProjMat = glm::perspectiveLH(mFov, mAspectRatio, mNearClip, mFarClip);
        }
    }
}

void FirstPersonCamera::translate(float x, float y, float z) {
    glm::mat4 rotMat = glm::eulerAngleX(mRot.x)*glm::eulerAngleY(mRot.y)*glm::eulerAngleZ(mRot.z);
    glm::vec4 move = glm::transpose(rotMat)*glm::vec4(x, y, z, 1.0f);
    setPosition(mPos + glm::vec3(move));
}

void FirstPersonCamera::translate(const glm::vec3 &offset) {
    translate(offset.x, offset.y, offset.z);
}

void FirstPersonCamera::rotate(float x, float y, float z) {
    setRotation(mRot.x + x, mRot.y + y, mRot.z + z);
}

void FirstPersonCamera::rotate(const glm::vec3 &angles) {
    rotate(angles.x, angles.y, angles.z);
}

void FirstPersonCamera::setRHMode(bool mode) {
    mIsRHMode = mode;
    mIsDirDirty = true;
    mIsViewDirty = true;
    mIsProjDirty = true;
}

void FirstPersonCamera::setPosition(float x, float y, float z) {
    mPos.x = x;
    mPos.y = y;
    mPos.z = z;
    mIsViewDirty = true;
}

void FirstPersonCamera::setPosition(const glm::vec3 & pos) {
	setPosition(pos.x, pos.y, pos.z);
}

void FirstPersonCamera::setRotation(float x, float y, float z) {
    mRot.x = glm::clamp(x, mMinVertical, mMaxVertical);

    float absY = glm::abs(y);
    if (absY > glm::two_pi<float>()) {
        int sign = static_cast<int>(y) / static_cast<int>(absY);
        mRot.y = (absY - glm::two_pi<float>())*sign;
    }
    else {
        mRot.y = y;
    }

    float absZ = glm::abs(z);
    if (absZ > glm::two_pi<float>()) {
        int sign = static_cast<int>(z) / static_cast<int>(absZ);
        mRot.z = (absZ - glm::two_pi<float>())*sign;
    }
    else {
        mRot.z = z;
    }

    mIsDirDirty = true;
    mIsViewDirty = true;
}

void FirstPersonCamera::setRotation(const glm::vec3 & rot) {
    setRotation(rot.x, rot.y, rot.z);
}

void FirstPersonCamera::setFov(float fov) {
    mFov = fov;
    mIsProjDirty = true;
}

void FirstPersonCamera::setAspectRatio(float aspectRatio) {
    mAspectRatio = aspectRatio;
    mIsProjDirty = true;
}

void FirstPersonCamera::setNearClip(float nearClip) {
    mNearClip = nearClip;
    mIsProjDirty = true;
}

void FirstPersonCamera::setFarClip(float farClip) {
    mFarClip = farClip;
    mIsProjDirty = true;
}

bool FirstPersonCamera::isRHMode() const {
    return mIsRHMode;
}

const glm::vec3 &FirstPersonCamera::getPosition() const {
	return mPos;
}

const glm::vec3 &FirstPersonCamera::getRotation() const {
	return mRot;
}

const glm::vec3 &FirstPersonCamera::getDirection() const {
    return mDir;
}

const glm::mat4 &FirstPersonCamera::getViewMatrix() const {
    return mViewMat;
}

const glm::mat4 &FirstPersonCamera::getProjMatrix() const {
    return mProjMat;
}

float FirstPersonCamera::getFov() const {
    return mFov;
}

float FirstPersonCamera::getAspectRatio() const {
    return mAspectRatio;
}

float FirstPersonCamera::getNearClip() const {
    return mNearClip;
}

float FirstPersonCamera::getFarClip() const {
    return mFarClip;
}

}