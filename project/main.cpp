
#include "game.h"
#include "heightfield.h"
#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif

#include <GL/glew.h>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <iostream>

#include <labhelper.h>
#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/euler_angles.hpp>
using namespace glm;

#include <Model.h>
#include "hdr.h"
#include "fbo.h"
#include "ParticleSystem.h"
#include "main.h"

using std::min;
using std::max;

///////////////////////////////////////////////////////////////////////////////
// Various globals
///////////////////////////////////////////////////////////////////////////////

const bool SHOW_HITBOX = false;
const std::string CAPTION = "Foreign Attack";
const int WIN_WIDTH = 1920;
const int WIN_HEIGHT = 1080;

SDL_Window* g_window = nullptr;
static float currentTime = 0.0f;
static float deltaTime = 0.0f;
bool showUI = true;
bool logger = false;
int antiAliasSamples = 16;
const float OPT_FRAMERATE = 60.f;
float previousTime = 0.0f;
int windowWidth, windowHeight;

// Mouse input
ivec2 g_prevMouseCoords = { -1, -1 };
bool g_isMouseDragging = false;
bool g_isMouseRightDragging = false;

//Ship
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

///////////////////////////////////////////////////////////////////////////////
// Shader programs
///////////////////////////////////////////////////////////////////////////////
GLuint shaderProgram;       // Shader for rendering the final image
GLuint simpleShaderProgram; // Shader used to draw the shadow map
GLuint simpleParticleProgram;
GLuint particleProgram;
GLuint backgroundProgram;
GLuint heightfieldProgram;

///////////////////////////////////////////////////////////////////////////////
// Environment
///////////////////////////////////////////////////////////////////////////////
float environmentMultiplier = 2.5f; // 1.5f - original code
GLuint environmentMap, irradianceMap, reflectionMap;
const std::string ENVMAP_BASE_NAME = "001";

///////////////////////////////////////////////////////////////////////////////
// Light source
///////////////////////////////////////////////////////////////////////////////
const vec3 LIGHT_POS_OFFSET (0, 50.0f, -40.0f);
vec3 lightPosition = LIGHT_POS_OFFSET;
float lightRotation = 0.f;
bool lightManualOnly = true;
vec3 pointLightColor = vec3(1.f, 1.f, 1.f);
bool useSpotLight = true;
float innerSpotlightAngle = 20.3f;
float outerSpotlightAngle = 24.0f;
float pointLightIntensityMultiplier = 10000.0f;

///////////////////////////////////////////////////////////////////////////////
// Shadow map
///////////////////////////////////////////////////////////////////////////////
enum ClampMode
{
	EDGE = 1,
	BORDER = 2
};

FboInfo shadowMapFB;
int shadowMapResolution = 1024;
int shadowMapClampMode = ClampMode::BORDER;
bool shadowMapClampBorderShadowed = false;
bool usePolygonOffset = true;
bool useSoftFalloff = false;
bool useHardwarePCF = true;
float polygonOffsetFactor = 5.83f;
float polygonOffsetUnits = 58.3f;

///////////////////////////////////////////////////////////////////////////////
// Camera parameters.
///////////////////////////////////////////////////////////////////////////////
vec3 cameraPosition(-140.0f, 40.0f, -40.0f); // -370.0f, 215.0f, 280.0f
vec3 cameraDirection = normalize(vec3(0.0f) - cameraPosition);
float cameraSpeed = 30.f;
float fieldOfView = 50.f;

vec3 worldUp(0.0f, 1.0f, 0.0f);
vec3 xAxis(1.0f, 0.0f, 0.0f);

///////////////////////////////////////////////////////////////////////////////
// Models
///////////////////////////////////////////////////////////////////////////////
labhelper::Model* fighterModel = nullptr;
labhelper::Model* landingpadModel = nullptr;
labhelper::Model* sphereModel = nullptr;

mat4 roomModelMatrix;
mat4 landingPadModelMatrix;
mat4 fighterModelMatrix;

///////////////////////////////////////////////////////////////////////////////
// Particle system
///////////////////////////////////////////////////////////////////////////////
ParticleSystem particleSystem;
const uint MAX_PARTICLES = 100000;

///////////////////////////////////////////////////////////////////////////////
// Heightfield
///////////////////////////////////////////////////////////////////////////////
const std::string HEIGHTFIELD_PATH = "../scenes/nlsFinland/L3123F.png";
const std::string TERRAIN_PHOTO_PATH = "../scenes/nlsFinland/L3123F_downscaled.jpg";
const int TERRAIN_TESSELATION = 1024;
const int TERRAIN_SCALING = 250;
HeightField terrain;

void loadShaders(const bool _isReload)
{
	GLuint shader = labhelper::loadShaderProgram("../project/simple.vert", "../project/simple.frag",
											 _isReload);
	if(shader) simpleShaderProgram = shader;

	shader = labhelper::loadShaderProgram("../project/background.vert", "../project/background.frag",
										  _isReload);
	if(shader) backgroundProgram = shader;

	shader = labhelper::loadShaderProgram("../project/shading.vert", "../project/shading.frag", _isReload);
	if(shader) shaderProgram = shader;

	shader = labhelper::loadShaderProgram("../project/particle.vert", "../project/particle.frag", _isReload);
	if (shader) particleProgram = shader;

	shader = labhelper::loadShaderProgram("../project/heightfield.vert", "../project/heightfield.frag", _isReload);
	if (shader) heightfieldProgram = shader;
}

void initShip(void) {
	if (FPS_ADJ_SPEED) acceleration *= FPS_ADJ_SPEED_CONST;
}

void initTerrain()
{
	terrain.generateMesh(TERRAIN_TESSELATION);
	terrain.loadHeightField(HEIGHTFIELD_PATH);
	terrain.loadDiffuseTexture(TERRAIN_PHOTO_PATH);
}

void initGL()
{

	///////////////////////////////////////////////////////////////////////
	//		Load Shaders
	///////////////////////////////////////////////////////////////////////
	backgroundProgram = labhelper::loadShaderProgram("../project/background.vert",
													 "../project/background.frag");
	shaderProgram = labhelper::loadShaderProgram("../project/shading.vert", "../project/shading.frag");
	simpleShaderProgram = labhelper::loadShaderProgram("../project/simple.vert", "../project/simple.frag");
	particleProgram = labhelper::loadShaderProgram("../project/particle.vert", "../project/particle.frag");
	heightfieldProgram = labhelper::loadShaderProgram("../project/heightfield.vert", "../project/shading.frag");

	initTerrain();

	particleSystem = ParticleSystem(MAX_PARTICLES);
	initShip();

	///////////////////////////////////////////////////////////////////////
	// Load models and set up model matrices
	///////////////////////////////////////////////////////////////////////
	fighterModel = labhelper::loadModelFromOBJ("../scenes/NewShip.obj");
	landingpadModel = labhelper::loadModelFromOBJ("../scenes/landingpad.obj");
	sphereModel = labhelper::loadModelFromOBJ("../scenes/sphere.obj");

	roomModelMatrix = mat4(1.0f);
	fighterModelMatrix = translate(Y_TRANSL * worldUp);
	landingPadModelMatrix = mat4(1.0f);

	///////////////////////////////////////////////////////////////////////
	// Load environment map
	///////////////////////////////////////////////////////////////////////
	const int roughnesses = 8;
	std::vector<std::string> filenames;
	for(auto i = 0; i < roughnesses; i++)
		filenames.push_back("../scenes/envmaps/" + ENVMAP_BASE_NAME + "_dl_" + std::to_string(i) + ".hdr");

	reflectionMap = labhelper::loadHdrMipmapTexture(filenames);
	environmentMap = labhelper::loadHdrTexture("../scenes/envmaps/" + ENVMAP_BASE_NAME + ".hdr");
	irradianceMap = labhelper::loadHdrTexture("../scenes/envmaps/" + ENVMAP_BASE_NAME + "_irradiance.hdr");


	glEnable(GL_DEPTH_TEST); // enable Z-buffering
	glEnable(GL_CULL_FACE);  // enables backface culling

	///////////////////////////////////////////////////////////////////////
	// Setup Framebuffer for shadow map rendering
	///////////////////////////////////////////////////////////////////////
	shadowMapFB.resize(shadowMapResolution, shadowMapResolution);

	glBindTexture(GL_TEXTURE_2D, shadowMapFB.depthBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
}

void drawLight(const glm::mat4& viewMatrix,
					const glm::mat4& projectionMatrix,
					const glm::vec3& worldSpaceLightPos)
{
	const auto modelMatrix = glm::translate(worldSpaceLightPos);
	glUseProgram(shaderProgram);
	labhelper::setUniformSlow(shaderProgram, "modelViewProjectionMatrix",
							  projectionMatrix * viewMatrix * modelMatrix);
	labhelper::render(sphereModel);
}

void drawBackground(const mat4& _viewMatrix, const mat4& _projectionMatrix)
{
	glUseProgram(backgroundProgram);
	labhelper::setUniformSlow(backgroundProgram, "environment_multiplier", environmentMultiplier);
	labhelper::setUniformSlow(backgroundProgram, "inv_PV", inverse(_projectionMatrix * _viewMatrix));
	labhelper::setUniformSlow(backgroundProgram, "camera_pos", cameraPosition);
	labhelper::drawFullScreenQuad();
}

void drawFire(const mat4& _projMatrix, const mat4& _viewMatrix, mat4& _fighterModelMatrix) {
	glUseProgram(particleProgram);
	labhelper::setUniformSlow(particleProgram, "projectionMatrix", _projMatrix);
	labhelper::setUniformSlow(particleProgram, "screen_x", float(windowWidth));
	labhelper::setUniformSlow(particleProgram, "screen_y", float(windowHeight));
	particleSystem.update(_viewMatrix, deltaTime, _fighterModelMatrix, accelerating);
}

void setLightUniforms(const GLuint _currentShaderProgram, const mat4& _viewMatrix, const mat4& _lightViewMatrix, const mat4& _lightProjectionMatrix,
	const vec4 _viewSpaceLightPosition)
{
	labhelper::setUniformSlow(_currentShaderProgram, "point_light_color", pointLightColor);
	labhelper::setUniformSlow(_currentShaderProgram, "point_light_intensity_multiplier",
		pointLightIntensityMultiplier);
	labhelper::setUniformSlow(_currentShaderProgram, "viewSpaceLightPosition", vec3(_viewSpaceLightPosition));
	labhelper::setUniformSlow(_currentShaderProgram, "viewSpaceLightDir",
		normalize(vec3(_viewMatrix * vec4(vec3(fighterModelMatrix[3]) - lightPosition, 0.0f))));
	labhelper::setUniformSlow(_currentShaderProgram, "spotOuterAngle", std::cos(radians(outerSpotlightAngle)));
	labhelper::setUniformSlow(_currentShaderProgram, "spotInnerAngle", std::cos(radians(innerSpotlightAngle)));

	const mat4 lightMatrix = translate(vec3(0.5f)) * scale(vec3(0.5f)) * _lightProjectionMatrix * _lightViewMatrix * inverse(_viewMatrix);
	labhelper::setUniformSlow(_currentShaderProgram, "lightMatrix", lightMatrix);

	// Environment
	labhelper::setUniformSlow(_currentShaderProgram, "environment_multiplier", environmentMultiplier);

	// camera
	labhelper::setUniformSlow(_currentShaderProgram, "viewInverse", inverse(_viewMatrix));
}

void setMatrixUniforms(const GLuint currentShaderProgram, const mat4& viewMatrix, const mat4& projectionMatrix, const mat4 modelMatrix)
{
	labhelper::setUniformSlow(currentShaderProgram, "modelViewProjectionMatrix",
		projectionMatrix * viewMatrix * modelMatrix);
	labhelper::setUniformSlow(currentShaderProgram, "modelViewMatrix", viewMatrix * modelMatrix);
	labhelper::setUniformSlow(currentShaderProgram, "normalMatrix",
		inverse(transpose(viewMatrix * modelMatrix)));
}

void drawTerrain(const mat4& _projMatrix, const mat4& _viewMatrix, const mat4& _lightViewMatrix, const mat4& _lightProjectionMatrix, const vec4& _viewSpaceLightPosition)
{
	mat4 modelMatrix({ TERRAIN_SCALING });
	modelMatrix[3] = vec4{ 0, 0, 0, 1.0 };
	glUseProgram(heightfieldProgram);
	glUniform1i(glGetUniformLocation(heightfieldProgram, "has_diffuse_texture"), 1);
	setLightUniforms(heightfieldProgram, _viewMatrix, _lightViewMatrix, _lightProjectionMatrix, _viewSpaceLightPosition);
	setMatrixUniforms(heightfieldProgram, _viewMatrix, _projMatrix, modelMatrix);
	terrain.submitTriangles();
}

void drawScene(const GLuint _currentShaderProgram,
               const mat4& _viewMatrix,
               const mat4& _projectionMatrix,
               const mat4& _lightViewMatrix,
               const mat4& _lightProjectionMatrix)
{
	const vec4 viewSpaceLightPosition = _viewMatrix * vec4(lightPosition, 1.0f);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, shadowMapFB.depthBuffer);

	drawTerrain(_projectionMatrix, _viewMatrix, _lightViewMatrix, _lightProjectionMatrix, viewSpaceLightPosition);

	glUseProgram(_currentShaderProgram);
	setLightUniforms(_currentShaderProgram, _viewMatrix, _lightViewMatrix, _lightProjectionMatrix, viewSpaceLightPosition);
	
	const mat4 modelMatrix(1.0f);
	setMatrixUniforms(_currentShaderProgram, _viewMatrix, _projectionMatrix, modelMatrix);
	render(landingpadModel);

	setMatrixUniforms(_currentShaderProgram, _viewMatrix, _projectionMatrix, fighterModelMatrix);
	render(fighterModel);
}

void drawFromCamera(const mat4 projMatrix, const mat4 viewMatrix, const mat4 lightViewMatrix, const mat4 lightProjMatrix)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawBackground(viewMatrix, projMatrix);
	drawScene(shaderProgram, viewMatrix, projMatrix, lightViewMatrix, lightProjMatrix);
	drawFire(projMatrix, viewMatrix, fighterModelMatrix);
	drawLight(viewMatrix, projMatrix, vec3(lightPosition));
}

void bindEnvironmentMaps()
{
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, environmentMap);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, irradianceMap);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, reflectionMap);
	glActiveTexture(GL_TEXTURE0);
}

void drawShadowMap(const mat4 _lightViewMatrix, const mat4 _lightProjMatrix)
{
	if (shadowMapFB.width != shadowMapResolution || shadowMapFB.height != shadowMapResolution)
	{
		shadowMapFB.resize(shadowMapResolution, shadowMapResolution);
	}
	
	if (shadowMapClampMode == ClampMode::EDGE)
	{
		glBindTexture(GL_TEXTURE_2D, shadowMapFB.depthBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	if (shadowMapClampMode == ClampMode::BORDER)
	{
		glBindTexture(GL_TEXTURE_2D, shadowMapFB.depthBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		vec4 border(shadowMapClampBorderShadowed ? 0.f : 1.f);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &border.x);
	}

	if (usePolygonOffset)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(polygonOffsetFactor, polygonOffsetUnits);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFB.framebufferId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, shadowMapFB.width, shadowMapFB.height);
	drawScene(simpleShaderProgram, _lightViewMatrix, _lightProjMatrix, _lightViewMatrix, _lightProjMatrix);
	labhelper::Material& screen = landingpadModel->m_materials[8];
	screen.m_emission_texture.gl_id = shadowMapFB.colorTextureTargets[0];

	if (usePolygonOffset)
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}

void display(void)
{
	{
		int w, h;
		SDL_GetWindowSize(g_window, &w, &h);
		if(w != windowWidth || h != windowHeight)
		{
			windowWidth = w;
			windowHeight = h;
		}
	}

	const mat4 projMatrix = perspective(radians(fieldOfView), float(windowWidth) / float(windowHeight), 5.0f, 900.0f);
	const mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraDirection, worldUp);

	const float lightRotationSpeed = 1.f;
	if (!lightManualOnly && !g_isMouseRightDragging)
	{
		lightRotation += deltaTime * lightRotationSpeed;
	}
	lightRotation += shipYRotationSpeed;
	lightPosition = vec3(rotate(lightRotation, worldUp) * vec4(LIGHT_POS_OFFSET, 1.f)) + vec3(fighterModelMatrix[3]);
	const mat4 lightViewMatrix = lookAt(lightPosition, vec3(0.0f), worldUp);
	const mat4 lightProjMatrix = perspective(radians(45.0f), 1.0f, 25.0f, 100.0f);

	bindEnvironmentMaps();

	drawShadowMap(lightViewMatrix, lightProjMatrix);

	drawFromCamera(projMatrix, viewMatrix, lightViewMatrix, lightProjMatrix);
}

bool handleEvents(void)
{
	// check events (keyboard among other)
	SDL_Event event;
	bool quitEvent = false;

	// Allow ImGui to capture events.
	ImGuiIO& io = ImGui::GetIO();

	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSdlGL3_ProcessEvent(&event);

		if (event.type == SDL_QUIT || (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
		{
			quitEvent = true;
		}
		if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_g)
		{
			showUI = !showUI;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN && (!showUI || !ImGui::GetIO().WantCaptureMouse)
			&& (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT)
			&& !(g_isMouseDragging || g_isMouseRightDragging))
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				g_isMouseDragging = true;
			}
			else if (event.button.button == SDL_BUTTON_RIGHT)
			{
				g_isMouseRightDragging = true;
			}
			int x;
			int y;
			SDL_GetMouseState(&x, &y);
			g_prevMouseCoords.x = x;
			g_prevMouseCoords.y = y;
		}

		if (!(SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
		{
			g_isMouseDragging = false;
		}
		if (!(SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT)))
		{
			g_isMouseRightDragging = false;
		}

		if (event.type == SDL_MOUSEMOTION)
		{
			// More info at https://wiki.libsdl.org/SDL_MouseMotionEvent
			const int deltaX = event.motion.x - g_prevMouseCoords.x;
			const int deltaY = event.motion.y - g_prevMouseCoords.y;
			if (g_isMouseDragging)
			{
				const float rotationSpeed = 0.005f;
				mat4 yaw = rotate(rotationSpeed * -deltaX, worldUp);
				mat4 pitch = rotate(rotationSpeed * -deltaY, normalize(cross(cameraDirection, worldUp)));
				cameraDirection = vec3(pitch * yaw * vec4(cameraDirection, 0.0f));
			}
			else if (g_isMouseRightDragging)
			{
				const float rotationSpeed = 0.01f;
				lightRotation += deltaX * rotationSpeed;
			}
			g_prevMouseCoords.x = event.motion.x;
			g_prevMouseCoords.y = event.motion.y;
		}
	}

	if (!io.WantCaptureKeyboard)
	{
		// check keyboard state (which keys are still pressed)
		const uint8_t* state = SDL_GetKeyboardState(nullptr);
		vec3 cameraRight = cross(cameraDirection, worldUp);
		if (state[SDL_SCANCODE_W])
		{
			cameraPosition += deltaTime * cameraSpeed * cameraDirection;
		}
		if (state[SDL_SCANCODE_S])
		{
			cameraPosition -= deltaTime * cameraSpeed * cameraDirection;
		}
		/*if (state[SDL_SCANCODE_A])
		{
			cameraPosition -= deltaTime * cameraSpeed * cameraRight;
		}
		if (state[SDL_SCANCODE_D])
		{
			cameraPosition += deltaTime * cameraSpeed * cameraRight;
		}*/
		if (state[SDL_SCANCODE_Q])
		{
			cameraPosition -= deltaTime * cameraSpeed * worldUp;
		}
		if (state[SDL_SCANCODE_E])
		{
			cameraPosition += deltaTime * cameraSpeed * worldUp;
		}
		/*if (state[SDL_SCANCODE_UP])
		{
			shipSpeed += acceleration;
			accelerating = true;
		}
		else accelerating = false;
		if (state[SDL_SCANCODE_DOWN])
		{
			shipSpeed -= acceleration;
		}
		if (state[SDL_SCANCODE_LEFT])
		{
			shipYRotationSpeed = MAX_SHIP_Y_ROTATION_SPEED;
			shipXRotationSpeed = shipXRotation >= MAX_SHIP_X_ROT ? 0 : MAX_SHIP_X_ROTATION_SPEED;
		}
		else if (state[SDL_SCANCODE_RIGHT])
		{
			shipYRotationSpeed = -MAX_SHIP_Y_ROTATION_SPEED;
			shipXRotationSpeed = shipXRotation <= -MAX_SHIP_X_ROT ? 0 : -MAX_SHIP_X_ROTATION_SPEED;
		}
		else
		{
			shipYRotationSpeed = 0;
			if (shipXRotation > CLAMP_ROT_TO_ZERO_SPEED) shipXRotationSpeed = -MAX_SHIP_X_ROTATION_SPEED;
			else if (shipXRotation < -CLAMP_ROT_TO_ZERO_SPEED) shipXRotationSpeed = MAX_SHIP_X_ROTATION_SPEED;
			else shipXRotationSpeed = 0;
		}*/
	}
	return quitEvent;
}

void gui()
{
	// Inform imgui of new frame
	ImGui_ImplSdlGL3_NewFrame(g_window);

	// ----------------- Set variables --------------------------
	if (ImGui::CollapsingHeader("Shadow variables", "shadow_ch", true, false))
	{
		ImGui::SliderInt("Shadow Map Resolution", &shadowMapResolution, 32, 2048);
		ImGui::Text("Polygon Offset");
		ImGui::Checkbox("Use polygon offset", &usePolygonOffset);
		ImGui::SliderFloat("Factor", &polygonOffsetFactor, 0.0f, 10.0f);
		ImGui::SliderFloat("Units", &polygonOffsetUnits, 0.0f, 100.0f);
		ImGui::Text("Clamp Mode");
		ImGui::RadioButton("Clamp to edge", &shadowMapClampMode, ClampMode::EDGE);
		ImGui::RadioButton("Clamp to border", &shadowMapClampMode, ClampMode::BORDER);
		ImGui::Checkbox("Border as shadow", &shadowMapClampBorderShadowed);
		ImGui::Checkbox("Use spot light", &useSpotLight);
		ImGui::Checkbox("Use soft falloff", &useSoftFalloff);
		ImGui::SliderFloat("Inner Deg.", &innerSpotlightAngle, 0.0f, 90.0f);
		ImGui::SliderFloat("Outer Deg.", &outerSpotlightAngle, 0.0f, 90.0f);
		ImGui::Checkbox("Use hardware PCF", &useHardwarePCF);
		ImGui::Checkbox("Manual light only (right-click drag to move)", &lightManualOnly);
	}
	
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
	            ImGui::GetIO().Framerate);
	ImGui::SliderFloat("Field of view: ", &fieldOfView, 10.0f, 70.0f);
	ImGui::SliderFloat("Acceleration", &acceleration, 0.0f, 10.0f);
	ImGui::SliderFloat("Drag coeff.", &dragCoeff, 0.0f, 10.0f);
	ImGui::Text("Current ship speed: %.3f", shipSpeed);
	ImGui::Text("Ship x-rotation: %.3f", shipXRotation);
	ImGui::Text("Ship x-rotation speed: %.3f", shipXRotationSpeed);
	ImGui::Text("Ship x-axis: %.3f %.3f %.3f", fighterModelMatrix[0].x, fighterModelMatrix[0].y, fighterModelMatrix[0].z);
	ImGui::Text("Ship y-axis: %.3f %.3f %.3f", fighterModelMatrix[1].x, fighterModelMatrix[1].y, fighterModelMatrix[1].z);
	ImGui::Text("Ship z-axis: %.3f %.3f %.3f", fighterModelMatrix[2].x, fighterModelMatrix[2].y, fighterModelMatrix[2].z);
	ImGui::Text("Camera position: %.3f %.3f %.3f", cameraPosition[0], cameraPosition[1], cameraPosition[2]);
	ImGui::SliderFloat("Exhaust x-offset", &exhXOffset, -20.0f, 20.0f);
	ImGui::SliderFloat("Exhaust y-offset", &exhYOffset, -20.0f, 20.0f);
	ImGui::SliderFloat("Exhaust z-offset", &exhZOffset, -2.0f, 2.0f);
	// ----------------------------------------------------------

	// Render the GUI.
	ImGui::Render();
}

void updateShip(void) {
	/*shipSpeed *= pow(dragCoeff, -abs(shipSpeed));
	if (FPS_ADJ_SPEED) shipSpeed = shipSpeed / (1000.f / deltaTime / 1000.f / OPT_FRAMERATE);
	if (xRotation)
	{
		if (shipXRotationSpeed == 0 && abs(shipXRotation) < CLAMP_ROT_TO_ZERO_SPEED) shipXRotation = 0.f;
		else shipXRotation += shipXRotationSpeed;
	}
	shipYRotation += shipYRotationSpeed;*/

	const mat4 rotMatrix = glm::eulerAngleYXZ(shipYRotation, shipXRotation, 0.f);
	const vec4 translation = fighterModelMatrix[3];
	fighterModelMatrix = rotMatrix * mat4(1.f);
	fighterModelMatrix[3] = translation;

	fighterModelMatrix = translate(fighterModelMatrix, shipSpeed * -xAxis);
	particleSystem.setExhaustOffset(vec3(exhXOffset, exhYOffset, exhZOffset));
}

void logStats() {
	std::cout << "fighterModelMatrix" << std::endl;
	for (size_t i = 0; i < fighterModelMatrix.length(); i++)
	{
		for (size_t j = 0; j < fighterModelMatrix.length(); j++)
		{
			std::cout << fighterModelMatrix[j][i] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "worldUp/local y angle: " << orientedAngle(worldUp, vec3(fighterModelMatrix[1]), xAxis) 
		<< std::endl;
	std::cout << "ShipZRotation: " << shipZRotation << std::endl;
	std::cout << "FPS: " << 1000.f / deltaTime / 1000.f << std::endl;
}

int main(int argc, char* argv[])
{
	
	g_window = labhelper::init_window_SDL("Foreign Attack", WIN_WIDTH, WIN_HEIGHT, antiAliasSamples);

	Engine engine;

	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	
	Game game(&engine, SHOW_HITBOX);
	
	initGL();

	auto quit = false;
	const auto startTime = std::chrono::system_clock::now();

	while(!game.isQuitting())
	{
		//update currentTime
		std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;
		previousTime = currentTime;
		currentTime = timeSinceStart.count();
		deltaTime = currentTime - previousTime;

		game.update(deltaTime);
		
		updateShip();
		// render to window
		display();

		// Render overlay GUI.
		if(showUI)
		{
			gui();
		}

		if (logger) logStats();

		// Swap front and back buffer. This frame will now been displayed.
		SDL_GL_SwapWindow(g_window);

		// check events (keyboard among other)
		//quit = handleEvents();
	}
	// Free Models
	freeModel(fighterModel);
	freeModel(landingpadModel);
	freeModel(sphereModel);

	// Shut down everything. This includes the window and all other subsystems.
	labhelper::shutDown(g_window);
	return 0;
}
