#pragma once
#include "../../project/component.h"

class RigidBody : public Component
{
	glm::vec3 acceleration{ 0 };
	glm::vec3 velocity{ 0 };
	
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

	bool accelerating = false;
	
	void applyDrag();

	void setRotation(bool _frozenPos, float& _rotVelocity, float _resetRotSpeedRot, float& _position) const;
	
public:

	void update(float _dt) override;
	RigidBody(GameObject* _go, const float _dragCoeff, const float _xAcceleration, const float _maxXRot, 
		const float _maxXRotSpeed, const float _maxYRotSpeed, const float _resetRotSpeed);
};