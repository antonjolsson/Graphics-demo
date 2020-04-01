#pragma once

#include <set>
#include <array>
#include "avancezlib.h"
#include "objectPool.h"
#include "gameObject.h"

class GameObject;

class Component {

protected:

    const Vector2D GRAVITY {0, 600};
    AvancezLib* engine;	// used to access the engine
	GameObject* go;		// the game object this component is part of
	std::set<GameObject*>* gameObjects;	// the global container of game objects

    bool paused = false;

public:

	virtual ~Component() = default;

	virtual void create(AvancezLib* _engine, GameObject* _go, std::set<GameObject*>* _gameObjects);

	virtual void init() {}
	virtual void update(float dt) = 0;
	virtual void receive(int message);
	virtual void destroy() {}
};


class RenderComponent : public Component
{
protected:

	const SpriteSheet MEGA_MAN_SHEET {
			"resource/sprites/megaman.png", {0, 102, 102, 255}};
	const SpriteSheet ENEMIES_SHEET {
			"resource/sprites/enemies.png", {109, 167, 131, 255}
	};

    Sprite* sprite;
    Vector2D spriteDims;

    struct SpriteProperties {
        float duration = 0;
        Vector2D sheetPosition;
        Vector2D dimensions;
    };
    SpriteProperties spriteProperties;

    float timeSinceModeSwitch = 0;

    struct Animation {
        float totalDuration = 0;
        std::vector<SpriteProperties> animationSprites;
        void init();
        SpriteProperties getCurrentSpriteProperties(float _timeSinceModeSwitch);
    };

    Animation* currentAnimation;

    void switchAnimation(RenderComponent::Animation& animation);

    Animation explosionAnimation {
            0,
            {{0.1, {648, 85}, {24, 24}},
             {0.1, {672, 85}, {24, 24}},
             {0.1, {696, 85}, {24, 24}},
             {0.1, {720, 85}, {24, 24}},
             {0.1, {744, 85}, {24, 24}},
            }};
public:

	void update(float dt) override;
	void destroy() override;

    void receive(int message) override;

    void terminate();

    bool determineToRender(float dt);
};

class EnemyRenderComponent : public RenderComponent {

public:
    void init() override;

    void update(float dt) override ;

    virtual void create(AvancezLib *_engine, GameObject *_go);

};

class BoxCollideComponent : public Component
{
    enum Edge {TOP, RIGHT, BOTTOM, LEFT, NO_EDGE};

	ObjectPool<GameObject>* collObjects; // collision will be tested with these objects
    std::array<Vector2D, 4> corners;
    std::array<Vector2D, 4> normals;
    bool debug = false;
    // Used to show the hit boxes
    Sprite* colliderSprite;
    SDL_Color outlineColor = {255, 100, 100, 255}; // {0, 255, 0, 255};

    Vector2D intersection {0, 0};

    Edge getIntersectingEdge(BoxCollideComponent* otherCollider);

public:
	virtual void create(AvancezLib *engine, GameObject *go, std::set<GameObject *> *gameObjects,
                        ObjectPool<GameObject> *_collObjects,
                        bool b);
	void update(float dt) override;

    Vector2D getMidPoint();

    Vector2D getDimensions();

    void setCorners(GameObject* go);

    void setNormals();

    void handleCollision(GameObject *otherObject, Edge edge);

    static Vector2D getIntersection(double xDistance, double yDistance, double boxXLengths, double boxYLengths);

    void clampIntersectingBox(GameObject *otherObject, Edge edge);

};

