#pragma once

#include "labhelper.h"

bool xRotation = true;
const bool FPS_ADJ_SPEED = false;
const float FPS_ADJ_SPEED_CONST = 1 / 5.f;
const float MAX_SHIP_X_ROT = M_PI / 6;;
const float MAX_SHIP_Y_ROTATION_SPEED = M_PI / 40;
const float MAX_SHIP_X_ROTATION_SPEED = MAX_SHIP_X_ROT / 5;
const float CLAMP_ROT_TO_ZERO_SPEED = 0.2f;
float acceleration = 0.8f;
float shipSpeed = 0.f;
float shipYRotationSpeed = 0.f;
float shipXRotationSpeed = 0.f;
float dragCoeff = 1.1f;
vec4 shipTranslation(0.f, 15.f, 0.f, 1.f);
const float Y_TRANSL = 15.0f;
float shipXRotation = 0.f;
float shipZRotation = 0.f;
float shipYRotation = 0.f;
float exhZOffset = 0.33f;
float exhYOffset = 3.1f;
float exhXOffset = 17.25f;
bool accelerating = false;

using namespace glm;