#include "imgui.h"
#include "imgui_impl_sdl_gl3.h"

namespace debug_gui {

	inline void showDebugGUI(SDL_Window* _gWindow, Ship* _ship, GameObject* _camera) {
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
		ImGui::SliderFloat("Field of view: ", &_camera->getComponent<CameraComponent>()->fieldOfView, 10.0f, 70.0f);
		
		/*ImGui::SliderFloat("Acceleration", &acceleration, 0.0f, 10.0f);
		ImGui::SliderFloat("Drag coeff.", &dragCoeff, 0.0f, 10.0f);*/
		const vec3 shipVelocity = _ship->getComponent<RigidBody>()->getVelocity();
		ImGui::Text("Current ship speed: %.3f %.3f %.3f", shipVelocity.x, shipVelocity.y, shipVelocity.z);
		const vec3 shipRotation = _ship->getTransform().rotation;
		ImGui::Text("Ship rotation: %.3f %.3f %.3f", shipRotation.x, shipRotation.y, shipRotation.z);
		ImGui::Text("Ship x-rotation speed: %.3f", _ship->getComponent<RigidBody>()->getRotationVelocity().x);
		const vec3 shipPosition = _ship->getTransform().position;
		ImGui::Text("Ship position: %.3f %.3f %.3f", shipPosition.x, shipPosition.y, shipPosition.z);
		mat4 fighterModelMatrix = _ship->getComponent<ModelRenderComponent>()->getModelMatrix();
		ImGui::Text("Ship x-axis: %.3f %.3f %.3f", fighterModelMatrix[0].x, fighterModelMatrix[0].y, fighterModelMatrix[0].z);
		ImGui::Text("Ship y-axis: %.3f %.3f %.3f", fighterModelMatrix[1].x, fighterModelMatrix[1].y, fighterModelMatrix[1].z);
		ImGui::Text("Ship z-axis: %.3f %.3f %.3f", fighterModelMatrix[2].x, fighterModelMatrix[2].y, fighterModelMatrix[2].z);
		const vec3 cameraPos = _camera->getTransform().position;
		ImGui::Text("Camera position: %.3f %.3f %.3f", cameraPos.x, cameraPos.y, cameraPos.z);
		
		// ----------------------------------------------------------

		// Render the GUI.
		ImGui::Render();
	}
}
