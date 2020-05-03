cmake_minimum_required ( VERSION 3.0.2 )

project ( project )

# Find *all* shaders.
file(GLOB_RECURSE SHADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.frag"
)
# Separate filter for shaders.
source_group("Shaders" FILES ${SHADERS})

# Build and link executable.
add_executable ( ${PROJECT_NAME}
    main.cpp
    fbo.h
    fbo.cpp
    hdr.cpp
    heightfield.cpp
    heightfield.h
    ParticleSystem.cpp
    ParticleSystem.h
    InputHandler.cpp
    InputHandler.h
    vector2D.h
    BehaviourComponent.h
    BehaviourComponent.cpp
    ship.h
    ship.cpp
    Renderer.h
    Renderer.cpp
    rigidBody.h
    rigidBody.cpp
    game.h
    AudioComponent.h
    AudioComponent.cpp
    gameObject.h
    CameraComponent.h
    CameraComponent.cpp
    ModelRenderComponent.h
    ModelRenderComponent.cpp
    Camera.cpp
    Camera.h
    LightComponent.h
    LightComponent.cpp
    ShadowMap.cpp
    ShadowMap.h
    component.h
    EnvironmentComponent.h
    EnvironmentComponent.cpp
    DebugGUI.h
    ${SHADERS}
     "component.cpp" "game.cpp" "gameObject.cpp")

target_link_libraries ( ${PROJECT_NAME} labhelper )
config_build_output()
