
#include <GL/glew.h>

#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <chrono>

#include <labhelper.h>
#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>
#include <Model.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <cmath>
#include <vector>
#include "lab3_main.h"

using namespace glm;
using namespace labhelper;

using std::min;
using std::max;

int old_w = 1280;
int old_h = 720;

struct PerspectiveParams
{
	float fov;
	int w;
	int h;
	float near;
	float far;
};

PerspectiveParams ppStatic = { 45.0f, 1280, 720, 0.1f, 300.0f };
PerspectiveParams pp1stPers = { 60.0f, 1280, 720, 0.60f, 300.0f };
PerspectiveParams* pp = &ppStatic;

// The window we'll be rendering to
SDL_Window* g_window = nullptr;

GLuint shaderProgram;

// Mouse input
ivec2 g_prevMouseCoords = { -1, -1 };
bool g_isMouseDragging = false;

float currentTime = 0.0f;
float deltaTime = 0.0f;
bool showUI = false;

// Models
Model* cityModel = nullptr;
Model* carModel = nullptr;
Model* groundModel = nullptr;
mat4 carModelMatrix1(1.0f), carModelMatrix2(1.0f);

float car2Speed = 0.14f;

vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);
vec3 zAxis(0, 0, 1.0f);

// Camera parameters
enum CameraType {STATIC, CAR_1ST_PERS, CAR_3D_PERS};
CameraType camera = CAR_3D_PERS;
vec3 cameraPosition(13.0f, 13.0f, 13.0f), statCamPos = cameraPosition;
vec3 cameraDirection(-1.0f, -1.0f, -1.0f), statCamDirection = cameraDirection;
mat4 T(1.0f), T2(1.0f), R(1.0f), R2(1.0f);
float zoomSpeed = 0.3f;
bool vKeyDown = false;
const vec3 CAR_1ST_PERS_OFFSET{ 0.2f, 2.0f, 0.47f };
const vec3 CAR_3RD_PERS_OFFSET{ 0.0f, 3.0f, -9.0f };

void initCar2(void) {
	carModelMatrix2[3] = vec4(-15.0f, 0, 0, 1);
}

void loadModels()
{
	///////////////////////////////////////////////////////////////////////////
	// Load models (both vertex buffers and textures).
	///////////////////////////////////////////////////////////////////////////
	cityModel = loadModelFromOBJ("../scenes/city.obj");
	carModel = loadModelFromOBJ("../scenes/car.obj");
	groundModel = loadModelFromOBJ("../scenes/ground_plane.obj");
	initCar2();
}

void drawGround(mat4 mvpMatrix)
{
	mvpMatrix[3] += vec4(0, 0.0005, 0, 0);
	int loc = glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix");
	glUniformMatrix4fv(loc, 1, false, &mvpMatrix[0].x);
	render(groundModel);
}

void moveCar1(void) {
	// check keyboard state (which keys are still pressed)
	const uint8_t* state = SDL_GetKeyboardState(nullptr);

	const float rotateSpeed = 2.f;
	if (state[SDL_SCANCODE_LEFT]) {
		R[0] -= rotateSpeed * deltaTime * R[2];
	}
	if (state[SDL_SCANCODE_RIGHT]) {
		R[0] += rotateSpeed * deltaTime * R[2];
	}

	// Make R orthonormal again
	R[0] = normalize(R[0]);
	R[2] = vec4(cross(vec3(R[0]), vec3(R[1])), 0.0f);

	// implement controls based on key states
	const float speed = 10.f;
	if (state[SDL_SCANCODE_UP]) {
		T[3] += speed * deltaTime * R[2];
	}
	if (state[SDL_SCANCODE_DOWN]) {
		T[3] -= speed * deltaTime * R[2];
	}
}

void moveCar2(void) {
	R2 = rotate(mat4(1.0f), (float) -(M_PI + currentTime / M_PI * 2), worldUp);
	carModelMatrix2[0] = R2[0];
	carModelMatrix2[2] = R2[2];
	carModelMatrix2 = translate(carModelMatrix2, car2Speed * zAxis);
}

void setCamParam(void) {
	switch (camera)
	{
	case CAR_1ST_PERS:
		vec4 offsVec1stPers = carModelMatrix1 * vec4{ CAR_1ST_PERS_OFFSET, 0 };
		cameraPosition = vec3{ carModelMatrix1[3][0], carModelMatrix1[3][1], carModelMatrix1[3][2] } + vec3{offsVec1stPers[0], offsVec1stPers[1] , offsVec1stPers[2]};
		cameraDirection = vec3{ carModelMatrix1[2][0], carModelMatrix1[2][1], carModelMatrix1[2][2] };
		pp = &pp1stPers;
		break;
	case CAR_3D_PERS:
		vec4 offsVec3rdPers = carModelMatrix1 * vec4{ CAR_3RD_PERS_OFFSET, 0 };
		cameraPosition = vec3{ carModelMatrix1[3][0], carModelMatrix1[3][1], carModelMatrix1[3][2] } + vec3{ offsVec3rdPers[0], offsVec3rdPers[1] , offsVec3rdPers[2] };
		cameraDirection = vec3{ carModelMatrix1[2][0], carModelMatrix1[2][1], carModelMatrix1[2][2]};
		pp = &ppStatic;
		break;
	default:
		cameraPosition = statCamPos;
		cameraDirection = statCamDirection;
		pp = &ppStatic;
		break;
	}
}

void display()
{
	// Set up
	int w, h;
	SDL_GetWindowSize(g_window, &w, &h);

	if(pp->w != old_w || pp->h != old_h)
	{
		SDL_SetWindowSize(g_window, pp->w, pp->h);
		w = pp->w;
		h = pp->h;
		old_w = pp->w;
		old_h = pp->h;
	}

	glViewport(0, 0, w, h);                             // Set viewport
	glClearColor(0.2f, 0.2f, 0.8f, 1.0f);               // Set clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the color buffer and the z-buffer
	glEnable(GL_DEPTH_TEST);                            // enable Z-buffering
	glDisable(GL_CULL_FACE);                            // disables not showing back faces of triangles

	// Set the shader program to use for this draw call
	glUseProgram(shaderProgram);

	// Set up model matrices
	mat4 cityModelMatrix(1.0f);

	setCamParam();

	// Set up the view matrix
	// The view matrix defines where the viewer is looking
	// Initially fixed, but will be replaced in the tutorial.
	mat4 constantViewMatrix = mat4(0.707106769f, -0.408248276f, 1.00000000f, 0.000000000f, 0.000000000f,
	                               0.816496551f, 1.00000000f, 0.000000000f, -0.707106769f, -0.408248276f,
	                               1.00000000f, 0.000000000f, 0.000000000f, 0.000000000f, -30.0000000f,
	                               1.00000000f);

	// use camera direction as -z axis and compute the x (cameraRight) and y (cameraUp) base vectors
	vec3 cameraRight = normalize(cross(cameraDirection, worldUp));
	vec3 cameraUp = normalize(cross(cameraRight, cameraDirection));
	mat3 cameraBaseVectorsWorldSpace(cameraRight, cameraUp, -cameraDirection);
	mat4 cameraRotation = mat4(transpose(cameraBaseVectorsWorldSpace));
	mat4 viewMatrix = cameraRotation * translate(-cameraPosition);

	// Setup the projection matrix
	if(w != old_w || h != old_h)
	{
		pp->h = h;
		pp->w = w;
		old_w = w;
		old_h = h;
	}
	mat4 projectionMatrix = perspective(radians(pp->fov), float(pp->w) / float(pp->h), pp->near, pp->far);

	// Concatenate the three matrices and pass the final transform to the vertex shader

	// City
	mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * cityModelMatrix;
	int loc = glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix");
	glUniformMatrix4fv(loc, 1, false, &modelViewProjectionMatrix[0].x);
	render(cityModel);

	// Ground
	// Task 5: Uncomment this
	//drawGround(modelViewProjectionMatrix);

	// cars
	moveCar1();
	carModelMatrix1 = T * R;
	modelViewProjectionMatrix = projectionMatrix * viewMatrix * carModelMatrix1;
	glUniformMatrix4fv(loc, 1, false, &modelViewProjectionMatrix[0].x);
	render(carModel);

	moveCar2();
	modelViewProjectionMatrix = projectionMatrix * viewMatrix * carModelMatrix2;
	glUniformMatrix4fv(loc, 1, false, &modelViewProjectionMatrix[0].x);
	render(carModel);

	glUseProgram(0);
}

void gui()
{
	// Inform imgui of new frame
	ImGui_ImplSdlGL3_NewFrame(g_window);

	// ----------------- Set variables --------------------------
	ImGui::SliderFloat("Field Of View", &pp->fov, 1.0f, 180.0f, "%.0f");
	ImGui::SliderInt("Width", &pp->w, 256, 1920);
	ImGui::SliderInt("Height", &pp->h, 256, 1080);
	ImGui::Text("Aspect Ratio: %.2f", float(pp->w) / float(pp->h));
	ImGui::SliderFloat("Near Plane", &pp->near, 0.1f, 300.0f, "%.2f", 2.f);
	ImGui::SliderFloat("Far Plane", &pp->far, 0.1f, 300.0f, "%.2f", 2.f);
	if(ImGui::Button("Reset"))
	{
		pp->fov = 45.0f;
		pp->w = 1280;
		pp->h = 720;
		pp->near = 0.1f;
		pp->far = 300.0f;
	}
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
	            ImGui::GetIO().Framerate);
	// ----------------------------------------------------------

	// Render the GUI.
	ImGui::Render();
}

void setCameraType()
{
	if (camera == CameraType::STATIC) {
		camera = CameraType::CAR_1ST_PERS;
	}
	else if (camera == CameraType::CAR_1ST_PERS) {
		camera = CameraType::CAR_3D_PERS;
	}
	else camera = CameraType::STATIC;
}

int main(int argc, char* argv[])
{
	g_window = labhelper::init_window_SDL("OpenGL Lab 3");

	// Load shader program
	shaderProgram = labhelper::loadShaderProgram("../lab3-camera/simple.vert", "../lab3-camera/simple.frag");

	// Load models
	loadModels();

	// render-loop
	bool stopRendering = false;
	auto startTime = std::chrono::system_clock::now();

	while(!stopRendering)
	{
		// update currentTime
		std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;
		deltaTime = timeSinceStart.count() - currentTime;
		currentTime = timeSinceStart.count();

		// render to window
		display();

		// Render overlay GUI.
		if(showUI)
		{
			gui();
		}

		// Swap front and back buffer. This frame will now been displayed.
		SDL_GL_SwapWindow(g_window);

		// check new events (keyboard among other)
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			// Allow ImGui to capture events.
			ImGui_ImplSdlGL3_ProcessEvent(&event);

			// More info at https://wiki.libsdl.org/SDL_Event
			if(event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
			{
				stopRendering = true;
			}
			if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_g)
			{
				showUI = !showUI;
			}
			else if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT
			        && (!showUI || !ImGui::GetIO().WantCaptureMouse))
			{
				g_isMouseDragging = true;
				int x;
				int y;
				SDL_GetMouseState(&x, &y);
				g_prevMouseCoords.x = x;
				g_prevMouseCoords.y = y;
			}

			if(!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)))
			{
				g_isMouseDragging = false;
			}

			if(event.type == SDL_MOUSEMOTION && g_isMouseDragging)
			{
				// More info at https://wiki.libsdl.org/SDL_MouseMotionEvent
				int delta_x = event.motion.x - g_prevMouseCoords.x;
				int delta_y = event.motion.y - g_prevMouseCoords.y;
				if(event.button.button == SDL_BUTTON_LEFT)
				{
					float rotationSpeed = 0.005f;
					mat4 yaw = rotate(rotationSpeed * -delta_x, worldUp);
					mat4 pitch = rotate(rotationSpeed * -delta_y, normalize(cross(statCamDirection, worldUp)));
					statCamDirection = vec3(pitch * yaw * vec4(cameraDirection, 0.0f));
				}
				g_prevMouseCoords.x = event.motion.x;
				g_prevMouseCoords.y = event.motion.y;
			}
		}

		// check keyboard state (which keys are still pressed)
		const uint8_t* state = SDL_GetKeyboardState(nullptr);

		// implement camera controls based on key states
		if(state[SDL_SCANCODE_W] && camera == STATIC)
		{
			statCamPos += statCamDirection * zoomSpeed;
		}
		if(state[SDL_SCANCODE_S] && camera == STATIC)
		{
			statCamPos -= statCamDirection * zoomSpeed;
		}
		if (state[SDL_SCANCODE_V] && !vKeyDown) {
			setCameraType();
			vKeyDown = true;
		}
		if (!state[SDL_SCANCODE_V]) vKeyDown = false;
	}

	// Shut down everything. This includes the window and all other subsystems.
	labhelper::shutDown(g_window);
	return 0;
}
