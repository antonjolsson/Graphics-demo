#pragma once

#include "gameObject.h"
#include "Ship.h"

class CameraComponent :public Component
{
	
	vec3 worldUp{ 0.0f, 1.0f, 0.0f };
	vec3 xAxis{ 1.0f, 0.0f, 0.0f };
	
	float fieldOfView = 50.f;
	float nearPlane = 5.f;
	float farPlane = 900.f;
	vec3 cameraDirection{};

public:
	void setCameraDirection(const vec3& _cameraDirection);
	void init(GameObject* _camera);
private:
	GameObject* tracing = nullptr;
	bool tracingObject;
	vec3 tracingDistance{ 50, 0, 0 };
	int windowWidth;
	int windowHeight;

public:
	
	CameraComponent(Ship* _tracing, int _winWidth, int _winHeight);

	void update(float _dt, int _windowHeight, int _windowWidth);
	mat4 getProjMatrix() const;
	mat4 getViewMatrix() const;
};

