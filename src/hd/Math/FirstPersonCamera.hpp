#pragma once
#include <glm/glm.hpp>

namespace hd {

class FirstPersonCamera {
public:
	FirstPersonCamera();

	void update(float dt);

	void translate(float x, float y, float z);
	void translate(const glm::vec3 &offset);
	void rotate(float x, float y, float z);
	void rotate(const glm::vec3 &angles);

	void setRHMode(bool mode);
	void setPosition(float x, float y, float z);
	void setPosition(const glm::vec3 &pos);
	void setRotation(float x, float y, float z);
	void setRotation(const glm::vec3 &rot);
	void setFov(float fov);
	void setAspectRatio(float aspectRatio);
	void setNearClip(float nearClip);
	void setFarClip(float farClip);

	bool isRHMode() const;
	const glm::vec3 &getPosition() const;
    const glm::vec3 &getRotation() const;
    const glm::vec3 &getDirection() const;
	const glm::mat4 &getViewMatrix() const;
	const glm::mat4 &getProjMatrix() const;
	float getFov() const;
	float getAspectRatio() const;
	float getNearClip() const;
	float getFarClip() const;

private:
	bool mIsRHMode;
	glm::vec3 mPos, mRot;
	glm::vec3 mDir;
	glm::mat4 mViewMat, mProjMat;
	float mMaxVertical, mMinVertical;
	float mFov, mAspectRatio, mNearClip, mFarClip;
	bool mIsDirDirty, mIsViewDirty, mIsProjDirty;
};

}