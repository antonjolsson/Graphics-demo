#pragma once

#include "../../project/gameObject.h"
#include "RenderComponent.h"
#include <GL/glew.h>
#include "ParticleSystem.h"
#include "engine.h"
#include "BehaviourComponent.h"
#include "rigidBody.h"

class Ship : public GameObject {

	labhelper::Model* fighterModel = labhelper::loadModelFromOBJ("../scenes/NewShip.obj");

	void readMessages() override;

public:
	
	const vec4 INITIAL_POSITION{ 0.f, 15.f, 0.f, 1.f };

	Ship(Engine* _engine, GLuint _shaderProgram, const bool _showHitbox);

	void update(float _dt) override;
};

class ShipBehaviour : public BehaviourComponent {

	const float DRAG_COEFF = 1.1f;
	const float X_ACCELERATION = 0.8f;
	const float MAX_SHIP_X_ROT = M_PI / 6;;
	const float MAX_SHIP_Y_ROTATION_SPEED = M_PI / 40;
	const float MAX_SHIP_X_ROTATION_SPEED = MAX_SHIP_X_ROT / 5;
	const float CLAMP_ROT_TO_ZERO_SPEED = 0.2f;

	Engine::KeyStatus keyStatus;
	RigidBody* rigidBody;

public:
	ShipBehaviour(Ship* _ship, Engine* _engine, RigidBody* _rigidBody);
	void update(float _dt) override;
};




