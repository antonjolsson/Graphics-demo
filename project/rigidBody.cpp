#include "rigidBody.h"

#include <glm/gtx/euler_angles.inl>
#include <glm/gtx/transform.inl>

void RigidBody::applyDrag()
{
	velocity.x *= pow(dragCoeff, -abs(velocity.x));
	velocity.y *= pow(dragCoeff, -abs(velocity.y));
	velocity.z *= pow(dragCoeff, -abs(velocity.z));
}

void RigidBody::setRotation(const bool _frozenPos, float& _rotVelocity, const float _resetRotSpeedRot, float& _position) const {
	if (!_frozenPos) {
		if (_rotVelocity == 0 && abs(_rotVelocity) < _resetRotSpeedRot) _rotVelocity = 0;
		else _position += _rotVelocity;
	}
}

void RigidBody::update(const float _dt) {
	glm::vec3& position = go->getTransform().position;
	glm::vec3& rotation = go->getTransform().rotation;
	if (accelerating) velocity += acceleration * _dt;
	applyDrag();
	setRotation(frozenPositions.x, rotationVelocity.x, resetRotSpeedRot.x,
		position.x);
	setRotation(frozenPositions.y, rotationVelocity.y, resetRotSpeedRot.y,
		position.y);
	setRotation(frozenPositions.z, rotationVelocity.z, resetRotSpeedRot.z,
		position.z);
}

RigidBody::RigidBody(GameObject* _go, const float _dragCoeff, const float _xAcceleration, const float _maxXRot, 
	const float _maxXRotSpeed, const float _maxYRotSpeed, const float _resetRotSpeed) {
	go = _go;
	dragCoeff = _dragCoeff;
	acceleration.x = _xAcceleration;
	maxRotation.x = _maxXRot;
	maxRotVelocity = { _maxXRotSpeed, _maxYRotSpeed, maxRotVelocity.z };
	resetRotSpeedRot = glm::vec3{ _resetRotSpeed };
}
