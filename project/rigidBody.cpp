#include "rigidBody.h"
#include <glm/detail/_vectorize.hpp>
#include <glm/detail/_vectorize.hpp>
#include <glm/gtx/euler_angles.inl>
#include <glm/gtx/transform.inl>


void RigidBody::setZeroAcc() {
	acceleration = glm::vec3(0);
}

void RigidBody::alterAcceleration(const glm::vec3 _vec) {
	acceleration.x += _vec.x;
	acceleration.y += _vec.y;
	acceleration.z += _vec.z;
}

void RigidBody::setRotationVelocity(glm::vec3 _velocity) {
	rotationVelocity = _velocity;
}

void RigidBody::setXRotationVel(const float _x) {
	rotationVelocity.x = _x;
}

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

RigidBody::RigidBody(GameObject* _go, const float _dragCoeff) {
	go = _go;
	dragCoeff = _dragCoeff;
}
