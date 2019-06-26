#pragma once
#include "../../3rd/include/glm/glm.hpp"

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
	//void AutoRotate(float speed = 0.2f);
	//void AutoMove(float speed = 0.2f, SDL_Scancode forward = SDL_SCANCODE_W, SDL_Scancode backward = SDL_SCANCODE_S,
	//	SDL_Scancode leftward = SDL_SCANCODE_A, SDL_Scancode rightward = SDL_SCANCODE_D);

	void setPosition(float x, float y, float z);
	void setPosition(const glm::vec3 &pos);
	void setRotation(float x, float y, float z);
	void setRotation(const glm::vec3 &rot);

	const glm::vec3 &getPosition() const;
    const glm::vec3 &getRotation() const;
	glm::mat4 getViewMatrixLH() const;
	glm::mat4 getViewMatrixRH() const;

private:
	glm::vec3 mPos;
	glm::vec3 mRot;
	glm::mat4 mViewMat;
	float mMaxVertical, mMinVertical;
};

}