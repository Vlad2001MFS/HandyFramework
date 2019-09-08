#pragma once
#include "glm/glm.hpp"

namespace hd {

class FirstPersonCamera
{
public:
	FirstPersonCamera();
	~FirstPersonCamera();

	void translate(float x, float y, float z);
	void translate(const glm::vec3 &offset);
	void rotate(float x, float y, float z);
	void rotate(const glm::vec3 &angles);

	void setPosition(float x, float y, float z);
	void setPosition(const glm::vec3 &pos);
	void setRotation(float x, float y, float z);
	void setRotation(const glm::vec3 &rot);

	const glm::vec3 &getPosition() const;
    const glm::vec3 &getRotation() const;
    glm::vec3 getDirectionLH() const;
    glm::vec3 getDirectionRH() const;
	glm::mat4 getViewMatrixLH() const;
	glm::mat4 getViewMatrixRH() const;

private:
	glm::vec3 mPos;
	glm::vec3 mRot;
	glm::mat4 mViewMat;
	float mMaxVertical, mMinVertical;
};

}