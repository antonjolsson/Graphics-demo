#pragma once

#include "gameObject.h"
#include "Ship.h"

class CameraComponent :public Component
{
	const float ROTATION_SPEED = 0.005f;
	InputHandler::KeyStatus keyStatus;
	InputHandler::MouseStatus mouse;
	float speed = 10;
	bool mouseMovable = false;
public:
	float fieldOfView = 50.f;
	InputHandler* inputHandler;
	void setFieldOfView(float _fieldOfView);
	float getNearPlane() const;
	float getFarPlane() const;
	void setMouseMovable(bool _mouseMovable);
private:
	float nearPlane = 5.f;
	float farPlane = 900.f;
	vec3 cameraDirection {0.f};

public:
	vec3 getCameraDirection() const;
	void setCameraDirection(const vec3& _cameraDirection);
	void init(GameObject* _camera);
private:
	GameObject* tracing = nullptr;
	bool tracingObject;
public:
	void setTracingObject(bool _tracingObject);
private:
	vec3 tracingDistance{ 50, 50, 0 }; // { 50, 50, 0 }
	int windowWidth;
	int windowHeight;

public:
	
	CameraComponent(GameObject* _tracing, int _winWidth, int _winHeight, InputHandler* _inputHandler);
	void traceObject();
	void moveCamera(float _dt);

	void update(float _dt, int _windowHeight, int _windowWidth);
	mat4 getProjMatrix() const;
	mat4 getViewMatrix() const;
};

