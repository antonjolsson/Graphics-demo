#pragma once

#include "../../project/gameObject.h"
#include "ModelRenderComponent.h"
#include <GL/glew.h>

#include "ParticleSystem.h"
#include "InputHandler.h"
#include "BehaviourComponent.h"
#include "rigidBody.h"

class Ship final : public GameObject {

	const float Y_TRANSL = 15.0f;
	labhelper::Model* fighterModel = labhelper::loadModelFromOBJ("../scenes/NewShip.obj");

	void readMessages() override;

public:
	
	const vec4 INITIAL_POSITION{ 0.f, 15.f, 0.f, 1.f };
	const vec3 EXHAUST_OFFSET{ 0.33, 3.1, 17.25 };

	Ship(InputHandler* _engine, GLuint _shaderProgram, const bool _showHitbox);

	void update(float _dt) override;
};

class ShipBehaviour final : public BehaviourComponent {

	const bool X_ROTATION = false;
	const float DRAG_COEFF = 1.1f;
	const float X_ACCELERATION = 0.020f;
	const float MAX_SHIP_X_ROT = M_PI / 6;;
	const float MAX_SHIP_Y_ROTATION_SPEED = M_PI / 100;
	const float MAX_SHIP_X_ROTATION_SPEED = MAX_SHIP_X_ROT / 5;
	const float CLAMP_ROT_TO_ZERO_SPEED = 0.2f;

	InputHandler::KeyStatus keyStatus;
	RigidBody* rigidBody;

public:
	
	ShipBehaviour(Ship* _ship, InputHandler* _inputhandler, RigidBody* _rigidBody);
	void update(float _dt) override;
};




