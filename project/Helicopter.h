#pragma once
#include "gameObject.h"
#include "Model.h"
#include <GL/glew.h>

#include "BehaviourComponent.h"
#include "rigidBody.h"

class Helicopter final : public GameObject {

	const float Y_TRANSL = 15.0f;
	labhelper::Model* helicopterModel = labhelper::loadModelFromOBJ("../scenes/mi-28.obj");

	void readMessages() override;

public:
	
	const glm::vec4 INITIAL_POSITION{ 0.f, 15.f, 0.f, 1.f };
	const glm::vec3 EXHAUST_OFFSET{ 0.33, 3.1, 17.25 };

	Helicopter(InputHandler* _engine, GLuint _shaderProgram, const bool _showHitbox);

	void update(float _dt) override;
};

class HelicopterBehaviour final : public BehaviourComponent {

	InputHandler::KeyStatus keyStatus;
	RigidBody* rigidBody;

public:

	const float CHOPPER_DRAG_COEFF = 0.99;
	const bool X_ROTATION = false;
	const float X_ACCELERATION = 0.1f;
	const float MAX_HELI_X_ROT = M_PI / 6;;
	const float MAX_HELI_Y_ROTATION_SPEED = M_PI / 100;
	const float MAX_HELI_X_ROTATION_SPEED = MAX_HELI_X_ROT / 5;
	const float CLAMP_ROT_TO_ZERO_SPEED = 0.2f;
	HelicopterBehaviour(Helicopter* _helicopter, InputHandler* _inputhandler, RigidBody* _rigidBody);
	void update(float _dt) override;
};