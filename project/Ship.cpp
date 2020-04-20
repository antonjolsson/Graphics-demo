#include "rigidBody.h"
#include "Ship.h"

#include "behaviour.h"

ShipBehaviour::ShipBehaviour(Ship* _ship, Engine* _engine, RigidBody* _rigidBody): Behaviour(_ship, _engine),
	keyStatus() {
	rigidBody = _rigidBody;
}

void ShipBehaviour::update(float _dt) {
	keyStatus = engine->getKeyStatus();
	if (keyStatus.quit) {
		go->send(QUIT);
		return;
	}
	if (keyStatus.forward) {
		rigidBody->alterAcceleration(vec3(X_ACCELERATION, 0, 0));
	}
	else rigidBody->setZeroAcc();
	if (keyStatus.reverse) {
		rigidBody->alterAcceleration(vec3(-X_ACCELERATION, 0, 0));
	}
	if (keyStatus.left) {
		rigidBody->setRotationVelocity(vec3(go->getTransform().rotation.x > MAX_SHIP_X_ROT ? 0 : MAX_SHIP_X_ROTATION_SPEED,
			MAX_SHIP_Y_ROTATION_SPEED, 0));
	}
	else if (keyStatus.right) {
		rigidBody->setRotationVelocity(vec3(go->getTransform().rotation.x <= MAX_SHIP_X_ROT ? 0 : MAX_SHIP_X_ROTATION_SPEED,
			-MAX_SHIP_Y_ROTATION_SPEED, 0));
	}
	else {
		rigidBody->setXRotationVel(0);
		if (go->getTransform().rotation.x > CLAMP_ROT_TO_ZERO_SPEED) rigidBody->setXRotationVel(-MAX_SHIP_X_ROTATION_SPEED);
		else if (go->getTransform().rotation.x < -CLAMP_ROT_TO_ZERO_SPEED) 
			rigidBody->setXRotationVel(MAX_SHIP_X_ROTATION_SPEED);
		else rigidBody->setXRotationVel(0);
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
}

Ship::Ship(Engine* _engine, GLuint _shaderProgram, const bool _showHitbox) {
	transform.position = INITIAL_POSITION;
	auto* renderer = new Renderer(_shaderProgram, fighterModel);
	auto* rigidBody = new RigidBody(this, DRAG_COEFF);
	auto* behaviour = new ShipBehaviour(this, _engine, rigidBody);
	addComponents(std::vector<Component*> {behaviour, renderer, rigidBody});
	enabled = true;
}

void Ship::update(const float _dt) {
	readMessages();
	GameObject::update(_dt);
}


