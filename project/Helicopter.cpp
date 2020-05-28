#include "Helicopter.h"

#include <iostream>

#include "ModelRenderComponent.h"
#include "ParticleSystem.h"
#include <glm/gtx/transform.hpp>

void Helicopter::readMessages() {
	for (Message m : mailbox) {
		switch (m) {
		case QUIT:
			send(m);
			break;
		default: ;
		}
	}
	mailbox.clear();
}

Helicopter::Helicopter(InputHandler* _engine, GLuint _shaderProgram, const bool _showHitbox) {
	transform.position = INITIAL_POSITION;
	modelMatrix = translate(Y_TRANSL * Y_AXIS);
	auto* renderer = new ModelRenderComponent(this, _shaderProgram, helicopterModel, modelMatrix);
	auto* rigidBody = new RigidBody(this, DRAG_COEFF);
	auto* behaviour = new HelicopterBehaviour(this, _engine, rigidBody);
	addComponents(std::vector<Component*> {behaviour, renderer, rigidBody});
	enabled = true;
}

void Helicopter::update(float _dt) {
	readMessages();
	GameObject::update(_dt);
}

HelicopterBehaviour::HelicopterBehaviour(Helicopter* _helicopter, InputHandler* _inputhandler, RigidBody* _rigidBody) :
BehaviourComponent(_helicopter, _inputhandler),
	keyStatus() {
	rigidBody = _rigidBody;
	rigidBody->setDragCoeff(CHOPPER_DRAG_COEFF);
	
}

void HelicopterBehaviour::update(float _dt) {
	keyStatus = inputHandler->getKeyStatus();
	if (keyStatus.quit) {
		go->send(QUIT);
		return;
	}
	if (keyStatus.toggleDebugGui) {
		go->send(TOGGLE_DEBUG_GUI);
		std::cout << "Chopper: toggleDebugGui = " << keyStatus.toggleDebugGui << std::endl;
	}
	if (keyStatus.forward) {
		rigidBody->addAcceleration(X_ACCELERATION * go->getModelMatrix()[0]);
	}
	if (keyStatus.reverse) {
		rigidBody->addAcceleration(-X_ACCELERATION * go->getModelMatrix()[0]);
	}
	if (keyStatus.left) {
		rigidBody->setRotationVelocity(glm::vec3(!X_ROTATION || go->getTransform().rotation.x > MAX_HELI_X_ROT ? 0 
			                                         : MAX_HELI_X_ROTATION_SPEED, MAX_HELI_Y_ROTATION_SPEED, 0));
	}
	else if (keyStatus.right) {
		rigidBody->setRotationVelocity(glm::vec3(!X_ROTATION || go->getTransform().rotation.x <= -MAX_HELI_X_ROT ? 0 :
			                                         -MAX_HELI_X_ROTATION_SPEED, -MAX_HELI_Y_ROTATION_SPEED, 0));
	}
	else {
		rigidBody->setYRotationVel(0);
		if (go->getTransform().rotation.x > CLAMP_ROT_TO_ZERO_SPEED) rigidBody->setXRotationVel(-MAX_HELI_X_ROTATION_SPEED);
		else if (go->getTransform().rotation.x < -CLAMP_ROT_TO_ZERO_SPEED) 
			rigidBody->setXRotationVel(MAX_HELI_X_ROTATION_SPEED);
		else rigidBody->setXRotationVel(0);

		if (rigidBody->getRotationVelocity().x == 0 && abs(go->getTransform().rotation.x) < CLAMP_ROT_TO_ZERO_SPEED)
			go->getTransform().rotation.x = 0;
		else go->getTransform().rotation.x += rigidBody->getRotationVelocity().x;
	}
	if (keyStatus.machinegun) {
		// TODO
	}
	if (keyStatus.missile) {
		// TODO
	}
}
