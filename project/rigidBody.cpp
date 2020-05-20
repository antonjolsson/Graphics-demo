#include "rigidBody.h"
#include <glm/detail/_vectorize.hpp>
#include <glm/detail/_vectorize.hpp>
#include <glm/gtx/euler_angles.inl>
#include <glm/gtx/transform.inl>
#include <iostream>


glm::vec3 RigidBody::getRotationVelocity() const {
	return rotationVelocity;
}

glm::vec3 RigidBody::getVelocity() const {
	return velocity;
}

void RigidBody::setYRotationVel(const float _y) {
	rotationVelocity.y = _y;
}

void RigidBody::setZeroAcc() {
	acceleration = glm::vec3(0);
}

void RigidBody::addAcceleration(const glm::vec3 _vec) {
	acceleration.x += _vec.x;
	acceleration.y += _vec.y;
	acceleration.z += _vec.z;
}

void RigidBody::setRotationVelocity(const glm::vec3 _velocity) {
	rotationVelocity = _velocity;
}

void RigidBody::setXRotationVel(const float _x) {
	rotationVelocity.x = _x;
}

void RigidBody::applyDrag(const float _dt)
{
	velocity.x *= pow(dragCoeff, -abs(velocity.x * _dt));
	velocity.y *= pow(dragCoeff, -abs(velocity.y) * _dt);
	velocity.z *= pow(dragCoeff, -abs(velocity.z) * _dt);
}

void RigidBody::setRotation(const bool _frozenPos, float& _rotVelocity, const float _resetRotSpeedRot, float& _rotation) const {
	if (!_frozenPos) {
		if (_rotVelocity == 0 && abs(_rotVelocity) < _resetRotSpeedRot) _rotVelocity = 0;
		else _rotation += _rotVelocity;
	}
}

void RigidBody::update(const float _dt) {
	glm::vec3& position = go->getTransform().position;
	glm::vec3& rotation = go->getTransform().rotation;
	
	position += velocity * _dt;
	
	//velocity += acceleration * _dt;
	velocity += acceleration;
	acceleration = glm::vec3(0);
	
	setRotation(frozenRotations.x, rotationVelocity.x, resetRotSpeedRot.x,
		rotation.x);
	setRotation(frozenRotations.y, rotationVelocity.y, resetRotSpeedRot.y,
		rotation.y);
	setRotation(frozenRotations.z, rotationVelocity.z, resetRotSpeedRot.z,
		rotation.z);

	applyDrag(_dt);
}

RigidBody::RigidBody(GameObject* _go, const float _dragCoeff) {
	go = _go;
	dragCoeff = _dragCoeff;
}
