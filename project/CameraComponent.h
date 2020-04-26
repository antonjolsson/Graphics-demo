#pragma once

#include "gameObject.h"
#include "Ship.h"

class CameraComponent :public Component
{
public:
	float fieldOfView = 50.f;
	void setFieldOfView(float _fieldOfView);
	float getNearPlane() const;
	float getFarPlane() const;
private:
	float nearPlane = 5.f;
	float farPlane = 900.f;
	vec3 cameraDirection{};

public:
	void setCameraDirection(const vec3& _cameraDirection);
	void init(GameObject* _camera);
private:
	GameObject* tracing = nullptr;
	bool tracingObject;
	vec3 tracingDistance{ 50, 50, 0 }; // { 50, 50, 0 }
	int windowWidth;
	int windowHeight;

public:
	
	CameraComponent(GameObject* _tracing, int _winWidth, int _winHeight);
	void traceObject();

	void update(float _dt, int _windowHeight, int _windowWidth);
	mat4 getProjMatrix() const;
	mat4 getViewMatrix() const;
};

