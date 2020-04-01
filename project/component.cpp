
#include <cmath>
#include <set>
#include "avancezlib.h"
#include "gameObject.h"
#include "component.h"

void Component::create(AvancezLib* _engine, GameObject* _go, std::set<GameObject*>* _gameObjects)
{
	go = _go;
	engine = _engine;
	gameObjects = _gameObjects;
}

void Component::receive(int message) {
    switch (message) { // NOLINT(hicpp-multiway-paths-covered)
    case GAME_OVER: case MSG_EXPLODED:
            paused = true;
            break;
        default:
            break;
    }
}

/*void RenderComponent2D::update(float dt)
{
	if (!go->enabled || paused)
		return;
    if (sprite) {
        sprite->draw(go->screenPosition, spriteProperties.sheetPosition,
                spriteProperties.dimensions); }
    go->spriteDims = spriteDims;
}

void RenderComponent2D::destroy() {
	if (sprite != nullptr)
		sprite->destroy();
	sprite = nullptr;
}*/

void BoxCollideComponent2D::create(AvancezLib *engine, GameObject *go, std::set<GameObject *> *gameObjects,
                                 ObjectPool<GameObject> *_collObjects, bool _debug) {
    setCorners(go);
    setNormals();
    Component::create(engine, go, gameObjects);
	this->collObjects = _collObjects;
	debug = _debug;
	//if (debug) colliderSprite = engine->createColliderSprite();
}

void BoxCollideComponent2D::setNormals() {
    for (int i = 0; i < normals.size(); i++) {
        normals[i] = (corners[(i + 1) % corners.size()] - corners[i]).getNormal();
    }
}

void BoxCollideComponent2D::setCorners(GameObject* go) {
    Vector2D ul {go->worldPosition}, br {go->worldPosition + go->spriteDims};
    corners = {ul, {br.x, ul.y}, br, {ul.x, br.y}};
}

void BoxCollideComponent2D::update(float dt) {
    if (paused) return;
    setCorners(go);
    setNormals();
	for (auto otherObject : collObjects->getPool()) {
        if (otherObject->enabled) {
            auto* otherCollider = otherObject->getComponent<BoxCollideComponent2D *>();
            if (otherCollider->paused)
                return;
            intersection = {0, 0};
            Edge edge = getIntersectingEdge(otherCollider);
            if (edge != Edge::NO_EDGE)
                handleCollision(otherObject, edge);
            else if (go->type == WATER && otherObject->type == PLAYER && otherObject->inWater)
                otherObject->receive(OUT_OF_WATER);
		}
	}
	/*if (debug) colliderSprite->drawOutline(go->screenPosition,
	        getDimensions(), outlineColor);*/
}

BoxCollideComponent2D::Edge BoxCollideComponent2D::getIntersectingEdge(BoxCollideComponent2D* otherCollider) {
    double xDistance = getMidPoint().x - otherCollider->getMidPoint().x;
    double yDistance = getMidPoint().y - otherCollider->getMidPoint().y;
    double boxXLengths = getDimensions().x + otherCollider->getDimensions().x;
    double boxYLengths = getDimensions().y + otherCollider->getDimensions().y;
    intersection = getIntersection(xDistance, yDistance, boxXLengths, boxYLengths);
    if (intersection.x > 0 && intersection.y > 0) {
        if (intersection.x < intersection.y) {
            if (xDistance > 0) {
                return LEFT;
            }
            return RIGHT;
        }
        else if (yDistance < 0) {
            return BOTTOM;
        }
        return TOP;
    }
    return NO_EDGE;
}

Vector2D BoxCollideComponent2D::getMidPoint() {
    return Vector2D {corners[0].x + getDimensions().x / 2, corners[0].y + getDimensions().y / 2};
}

Vector2D BoxCollideComponent2D::getDimensions() {
    return Vector2D {corners[1].x - corners[0].x, corners[2].y - corners[1].y};
}

void BoxCollideComponent2D::handleCollision(GameObject *otherObject, Edge edge) {
    switch (go->type) {
        case GROUND: case ICE:
            clampIntersectingBox(otherObject, edge);
            go->receive(otherObject->objectHitMessageMap.at(otherObject->type));
            if (edge == TOP) otherObject->receive(go->objectHitMessageMap.at(go->type));
            break;
        case WATER:
            if (edge == TOP && !otherObject->inWater)
                otherObject->receive(go->objectHitMessageMap.at(go->type));
            break;
        case TRAPDOOR:
            if ((otherObject->type == PLAYER || otherObject->type == CRAZY_RAZY) &&
                !otherObject->inAir && intersection.x > otherObject->spriteDims.x) {
                otherObject->receive(OBJ_IN_AIR);
            }
            break;
        case ENEMY: case CRAZY_RAZY: case PEPE: // Enemy hit
            go->receive(otherObject->objectHitMessageMap.at(otherObject->type));
            otherObject->receive(go->objectHitMessageMap.at(ENEMY));
            break;
        case PLAYER: // Player hit by enemy
            if (go->isInvincibleAfterHit() || go->isLevelWon())
                break;
            go->decreaseHP(otherObject->getAttackDamage());
            otherObject->receive(go->objectHitMessageMap.at(PLAYER));
            switch (edge) {
                case RIGHT:
                    go->receive(HIT_FROM_RIGHT);
                    break;
                case LEFT:
                    go->receive(HIT_FROM_LEFT);
                    break;
                case TOP:
                    go->receive(HIT_FROM_TOP);
                    break;
                case BOTTOM:
                    go->receive(HIT_FROM_BOTTOM);
                    break;
                default:
                    break;
            }
            clampIntersectingBox(otherObject, edge);
            break;
        default:
            break;
    }
}

// Remove boxes' intersection
void BoxCollideComponent2D::clampIntersectingBox(GameObject *otherObject, Edge edge) {
    switch (edge) {
        case LEFT:
            otherObject->worldPosition.x = go->worldPosition.x - otherObject->spriteDims.x;
            otherObject->velocity.x = 0;
            break;
        case RIGHT:
            otherObject->worldPosition.x = go->worldPosition.x + getDimensions().x;
            otherObject->velocity.x = 0;
            break;
        case TOP:
            otherObject->worldPosition.y = go->worldPosition.y - otherObject->spriteDims.y;
            otherObject->velocity.y = 0;
            break;
        case BOTTOM:
            otherObject->worldPosition.y = go->worldPosition.y + getDimensions().y;
            otherObject->velocity.y = 0;
            break;
        case NO_EDGE:
            break;
    }
}

Vector2D BoxCollideComponent2D::getIntersection(double xDistance, double yDistance, double boxXLengths,
                                              double boxYLengths) {
    return Vector2D {boxXLengths / 2 - abs(xDistance), boxYLengths / 2 - abs(yDistance)};
}

/*void RenderComponent2D::Animation::init() {
    for (auto& entry : animationSprites) {
        totalDuration += entry.duration;
    }
}

RenderComponent2D::SpriteProperties RenderComponent2D::Animation::getCurrentSpriteProperties(float _timeSinceModeSwitch) {
    float timeSum = 0;
    _timeSinceModeSwitch = std::fmod(_timeSinceModeSwitch, totalDuration);
    for (auto& entry : animationSprites) {
        timeSum += entry.duration;
        if (_timeSinceModeSwitch < timeSum)
            return entry;
    }
    return animationSprites[animationSprites.size() - 1];
}

void RenderComponent2D::switchAnimation(RenderComponent2D::Animation& animation) {
    timeSinceModeSwitch = 0;
    currentAnimation = &animation;
}

void RenderComponent2D::receive(int message) {
    switch (message) {
        case MSG_EXPLODED: case PLAYER_KILLED:
            switchAnimation(explosionAnimation);
            //sprite = engine->createSprite(MEGA_MAN_SHEET);
            break;
        default:
            break;
    }
}

void EnemyRenderComponent::create(AvancezLib *_engine, GameObject *_go) {
    engine = _engine;
    go = _go;
    //sprite = engine->createSprite(ENEMIES_SHEET);
    spriteDims = currentAnimation->getCurrentSpriteProperties(timeSinceModeSwitch).dimensions;
    go->worldPosition -= spriteDims;
}

void EnemyRenderComponent::init() {
    timeSinceModeSwitch = 0;
    currentAnimation->init();
    explosionAnimation.init();
    Component::init();
}

void EnemyRenderComponent::update(float dt) {
     if (!determineToRender(dt)) return;

    spriteProperties = currentAnimation->getCurrentSpriteProperties(timeSinceModeSwitch);
    if (sprite) sprite->draw(go->screenPosition, spriteProperties.sheetPosition,
                             spriteProperties.dimensions, spriteProperties.dimensions, go->facingRight);
    // Crazy Razy has two explosion animations
    if (go->mode == EXPLODING_MODE && go->type == CRAZY_RAZY && sprite)
        sprite->draw(go->screenPosition + Vector2D{0, spriteProperties.dimensions.y / 2},
                spriteProperties.sheetPosition,spriteProperties.dimensions,
                spriteProperties.dimensions, go->facingRight);
    go->spriteDims = spriteDims;
}

void RenderComponent2D::terminate() {
    paused = true;
    go->enabled = false;
    go->destroyedForever = true;
}

bool RenderComponent2D::determineToRender(float dt) {
    if (!go->enabled || paused) return false; // Stop rendering player if paused (i.e. game over)
    timeSinceModeSwitch += dt;
    // Stop rendering after explosion is over
    if (go->mode == EXPLODING_MODE && timeSinceModeSwitch > currentAnimation->totalDuration) {
        terminate();
        return false;
    }
    return true;
}*/