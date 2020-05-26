#pragma once

#include <glm/mat4x2.hpp>
#include "gameObject.h"

class CameraComponent :public Component
{
	const float ROTATION_SPEED = 0.005f;
	InputHandler::KeyStatus keyStatus{};
	InputHandler::MouseStatus mouse;
	float speed = 10;
	bool mouseMovable = false;
	bool toggleCameraButtonDown = false;
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
	glm::vec3 staticCameraDirection {0.f};
	glm::vec3 cameraDirection {0.f};

public:
	glm::vec3 getCameraDirection() const;
	void setCameraDirection(const glm::vec3& _cameraDirection);
	void init(GameObject* _camera);
private:
	GameObject* tracing = nullptr;
	bool tracingObject;
public:
	void setTracingObject(bool _tracingObject);
private:
	glm::vec3 staticCameraPos { 50, 50, 0 }; 
	glm::vec3 tracingDistance{ 55, 25, 0 }; // { 50, 50, 0 }
	glm::vec3 tracingDirectionOffs {0, 10, 0};
	
	int windowWidth;
	int windowHeight;

public:
	
	CameraComponent(GameObject* _tracing, int _winWidth, int _winHeight, InputHandler* _inputHandler);
	void traceObject();
	void moveCamera(float _dt);

	void update(float _dt, int _windowHeight, int _windowWidth);
	glm::mat4 getProjMatrix() const;
	glm::mat4 getViewMatrix() const;
};

