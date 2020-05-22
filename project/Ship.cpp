#include "rigidBody.h"
#include "Ship.h"

#include <glm/gtx/transform.hpp>
#include <iostream>

#include "BehaviourComponent.h"

ShipBehaviour::ShipBehaviour(Ship* _ship, InputHandler* _inputhandler, RigidBody* _rigidBody):
BehaviourComponent(_ship, _inputhandler),
	keyStatus() {
	rigidBody = _rigidBody;
	rigidBody->setDragCoeff(SHIP_DRAG_COEFF);
}

void ShipBehaviour::update(float _dt) {
	keyStatus = inputHandler->getKeyStatus();
	if (keyStatus.quit) {
		go->send(QUIT);
		return;
	}
	if (keyStatus.toggleDebugGui) {
		go->send(TOGGLE_DEBUG_GUI);
		std::cout << "Ship: toggleDebugGui = " << keyStatus.toggleDebugGui << std::endl;
	}
	if (keyStatus.forward) {
		rigidBody->addAcceleration(X_ACCELERATION * go->getModelMatrix()[0]);
	}
	if (keyStatus.reverse) {
		rigidBody->addAcceleration(-X_ACCELERATION * go->getModelMatrix()[0]);
	}
	if (keyStatus.left) {
		rigidBody->setRotationVelocity(vec3(!X_ROTATION || go->getTransform().rotation.x > MAX_SHIP_X_ROT ? 0 
			: MAX_SHIP_X_ROTATION_SPEED, MAX_SHIP_Y_ROTATION_SPEED, 0));
	}
	else if (keyStatus.right) {
		rigidBody->setRotationVelocity(vec3(!X_ROTATION || go->getTransform().rotation.x <= -MAX_SHIP_X_ROT ? 0 :
			-MAX_SHIP_X_ROTATION_SPEED, -MAX_SHIP_Y_ROTATION_SPEED, 0));
	}
	else {
		rigidBody->setYRotationVel(0);
		if (go->getTransform().rotation.x > CLAMP_ROT_TO_ZERO_SPEED) rigidBody->setXRotationVel(-MAX_SHIP_X_ROTATION_SPEED);
		else if (go->getTransform().rotation.x < -CLAMP_ROT_TO_ZERO_SPEED) 
			rigidBody->setXRotationVel(MAX_SHIP_X_ROTATION_SPEED);
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

void Ship::readMessages() {
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

Ship::Ship(InputHandler* _engine, const GLuint _shaderProgram, const bool _showHitbox) {
	transform.position = INITIAL_POSITION;
	modelMatrix = translate(Y_TRANSL * Y_AXIS);
	auto* renderer = new ModelRenderComponent(this, _shaderProgram, fighterModel, modelMatrix);
	auto* rigidBody = new RigidBody(this, DRAG_COEFF);
	auto* behaviour = new ShipBehaviour(this, _engine, rigidBody);
	addComponents(std::vector<Component*> {behaviour, renderer, rigidBody});
	enabled = true;
}

void Ship::update(const float _dt) {
	readMessages();
	GameObject::update(_dt);
}


