#pragma once
#include "../../project/component.h"

class RigidBody : public Component
{
	glm::vec3 acceleration{ 0 };
	glm::vec3 velocity{ 0 };

public:
	glm::vec3 getVelocity() const;
private:
	glm::vec3 rotationVelocity{ 0 };

	glm::vec3 maxRotVelocity{ FLT_MAX };
	glm::vec3 maxRotation{ M_PI };
	glm::vec3 resetRotSpeedRot{ 0 };

	struct 
	{
		bool x = false;
		bool y = false;
		bool z = false;
	} frozenPositions;

	struct
	{
		bool x = false;
		bool y = false;
		bool z = false;
	} frozenRotations;


	//float mass = 1; TODO: Implement
	float dragCoeff = 0;

	//bool useGravity = false; TODO: Implement

public:
	void setZeroAcc();
	void addAcceleration(glm::vec3 _vec);
	void setRotationVelocity(glm::vec3 _velocity);
	void setXRotationVel(float _x);
	glm::vec3 getRotationVelocity() const;
private:
	void applyDrag();

	void setRotation(bool _frozenPos, float& _rotVelocity, float _resetRotSpeedRot, float& _rotation) const;
	
public:

	void update(float _dt) override;
	RigidBody(GameObject* _go, float _dragCoeff);
};
