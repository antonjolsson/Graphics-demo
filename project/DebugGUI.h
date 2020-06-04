#include "imgui.h"
#include "imgui_impl_sdl_gl3.h"

namespace debug_gui {

	inline void showDebugGUI(SDL_Window* _gWindow, Ship* _ship, GameObject* _camera, GameObject* _sun, Renderer* _renderer) {
		auto* cameraComp = _camera->getComponent<CameraComponent>();
			// Inform imgui of new frame
		ImGui_ImplSdlGL3_NewFrame(_gWindow);

		// ----------------- Set variables --------------------------
		/*if (ImGui::CollapsingHeader("Shadow variables", "shadow_ch", true, false))
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
		}*/
		
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
		            ImGui::GetIO().Framerate);
		if (ImGui::CollapsingHeader("Camera", "camera", true, false)) {
			ImGui::SliderFloat("Field of view: ", &cameraComp->fieldOfView, 10.0f, 70.0f);
			const vec3 cameraPos = _camera->getTransform().position;
			const vec3 cameraDir = cameraComp->getCameraDirection();
			ImGui::Text("Camera position: %.3f %.3f %.3f", cameraPos.x, cameraPos.y, cameraPos.z);
			ImGui::Text("Camera direction: %.3f %.3f %.3f", cameraDir.x, cameraDir.y, cameraDir.z);
		}

		if (ImGui::CollapsingHeader("Ship", "ship", true, false)) {
			/*ImGui::SliderFloat("Acceleration", &acceleration, 0.0f, 10.0f);
			ImGui::SliderFloat("Drag coeff.", &dragCoeff, 0.0f, 10.0f);*/
			const vec3 shipVelocity = _ship->getComponent<RigidBody>()->getVelocity();
			ImGui::Text("Current ship speed: %.3f %.3f %.3f", shipVelocity.x, shipVelocity.y, shipVelocity.z);
			const vec3 shipAcceleration = _ship->getComponent<RigidBody>()->getAcceleration();
			ImGui::Text("Current ship acc: %.3f %.3f %.3f", shipAcceleration.x, shipAcceleration.y, shipAcceleration.z);
			const vec3 shipRotation = _ship->getTransform().rotation;
			ImGui::Text("Ship rotation: %.3f %.3f %.3f", fmod(shipRotation.x, 2 * M_PI), fmod(shipRotation.y, 2 * M_PI), 
				fmod(shipRotation.z, 2 * M_PI));
			ImGui::Text("Ship x-rotation speed: %.3f", _ship->getComponent<RigidBody>()->getRotationVelocity().x);
			ImGui::Text("Ship y-rotation speed: %.3f", _ship->getComponent<RigidBody>()->getRotationVelocity().y);
			const vec3 shipPosition = _ship->getTransform().position;
			ImGui::Text("Ship position: %.3f %.3f %.3f", shipPosition.x, shipPosition.y, shipPosition.z);
			mat4 fighterModelMatrix = _ship->getComponent<ModelRenderComponent>()->getModelMatrix();
			ImGui::Text("Ship x-axis: %.3f %.3f %.3f", fighterModelMatrix[0].x, fighterModelMatrix[0].y, fighterModelMatrix[0].z);
			ImGui::Text("Ship y-axis: %.3f %.3f %.3f", fighterModelMatrix[1].x, fighterModelMatrix[1].y, fighterModelMatrix[1].z);
			ImGui::Text("Ship z-axis: %.3f %.3f %.3f", fighterModelMatrix[2].x, fighterModelMatrix[2].y, fighterModelMatrix[2].z);
		}

		if (ImGui::CollapsingHeader("Sun", "sun", true, false)) {
			vec3 sunPos = _sun->getPosition();
			ImGui::SliderFloat("Sun x-pos: ", &sunPos.x, -300.0f, 300.0f);
			ImGui::SliderFloat("Sun y-pos: ", &sunPos.y, -300.0f, 300.0f);
			ImGui::SliderFloat("Sun z-pos: ", &sunPos.z, -300.0f, 300.0f);
			_sun->setPosition(sunPos);
		}

		if (ImGui::CollapsingHeader("Fog", "fog", true, false)) {
			ImGui::Checkbox("Fog on", &_renderer->fog);
			ImGui::SliderFloat("Fog density: ", &_renderer->fogDensity, 0, 1);
			ImGui::SliderFloat("Depth range: ", &_renderer->depthRange, 100, 400);
		}
		if (ImGui::CollapsingHeader("Depth-of-field", "dof", true, false)) {
			ImGui::Checkbox("Depth-of-field on", &_renderer->depthOfField);
			ImGui::SliderFloat("Aperture size: ", &_renderer->aperture, 0.01, 1);
		}

		if (ImGui::CollapsingHeader("SSAO", "ssao", false, false)) {
			ImGui::Checkbox("SSAO on", &_renderer->ssao);
			ImGui::Checkbox("Blurred SSAO", &_renderer->blurredSSAO);
			ImGui::Checkbox("Show only SSAO", &_renderer->showOnlySSAO);
			ImGui::SliderFloat("Radius: ", &_renderer->ssaoRadius, 0, 5);
			ImGui::SliderInt("Samples: ", &_renderer->ssaoSamples, 0, 50);
			ImGui::SliderFloat("Intensity: ", &_renderer->ssaoIntensity, 0, 5);
		}

		if (ImGui::CollapsingHeader("Tone mapping", "tone", true, false)) {
			ImGui::Checkbox("Tone mapping on", &_renderer->toneMapping);
			ImGui::SliderFloat("Gamma: ", &_renderer->gamma, 0, 5);
			ImGui::SliderFloat("Exposure: ", &_renderer->exposure, 0, 5);
		}
		
		// ----------------------------------------------------------

		// Render the GUI.
		ImGui::Render();
	}
}
