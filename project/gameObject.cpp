
#include "avancezlib.h"
#include "gameObject.h"

void GameObject::create() {
	enabled = false;
}

void GameObject::addComponent(Component* component) {
	components.push_back(component);
}

void GameObject::addComponents(const std::vector<Component*>& _components) {
    for (auto comp : _components)
        addComponent(comp);
}

void GameObject::init() {
	for (auto& component : components)
        component->init();
	enabled = true;
}

void GameObject::update(float dt, Vector2D viewPortPos) {
	if (!enabled) return;
    if (type != PLAYER) screenPosition = worldToScreenPos(viewPortPos);
	for (auto& component : components) {
        component->update(dt);
	}
}

void GameObject::destroy() {
	for (auto& component : components)
        component->destroy();
}

GameObject::~GameObject() = default;

void GameObject::addReceiver(GameObject* go) {
	receivers.push_back(go);
}

void GameObject::send(Message m) {
	for (auto& receiver : receivers) {
		if (!receiver->enabled)
			continue;
        receiver->receive(m);
	}
}

void GameObject::receive(Message m) {
    if (!enabled) return;
    switch (m) {
        case MOVED_LEFT: case MOVED_RIGHT:
            sendComponents(m);
            if (mode != IN_AIR && mode != IN_AIR_FIRING && mode != RUNNING_FIRING && mode != TAKEN_DAMAGE && !inAir)
                mode = RUNNING;
            break;
        case NO_INPUT:
            sendComponents(m);
            if (!inAir)
                mode = IDLE;
            break;
        case FIRE:
            if (mode == IDLE) {
                mode = FIRING;
            }
            else if (mode == RUNNING){
                mode = RUNNING_FIRING;
            } else if (mode == IN_AIR)
                mode = IN_AIR_FIRING;
            sendComponents(m);
            break;
        case BULLET_HIT: case PLAYER_HIT: // Enemy hit
            if (type != CRAZY_RAZY && type != PEPE)
                break;
            playSound(ENEMY_HIT_SOUND, HIT_SOUND_VOL);
            if (--healthPoints <= 0) {
                mode = EXPLODING_MODE;
                send(type == CRAZY_RAZY ? CRAZY_KILLED : PEPE_KILLED);
                sendComponents(MSG_EXPLODED);
                return;
            }
            sendComponents(m);
            break;
        case OBJ_IN_AIR:
            mode = IN_AIR;
            sendComponents(m);
            break;
        case GAME_OVER:
            gameOver = true;
            sendComponents(m);
            break;
        case LEVEL_WON:
            levelWon = true;
            break;
        default: break;
    }
}

void GameObject::sendComponents(Message message) {
    for (auto& component : components)
        component->receive(message);
}

Vector2D GameObject::worldToScreenPos(Vector2D viewPortPos) {
    return worldPosition - viewPortPos;
}

void GameObject::create(unsigned int _gameWidth) {
    gameWidth = _gameWidth;
}

void GameObject::setLevelWidth(unsigned int _levelWidth) {
    levelWidth = _levelWidth;
}

unsigned int GameObject::getLevelWidth() const {
    return levelWidth;
}

int GameObject::getAttackDamage() {
    return attackDamage;
}

bool GameObject::isInvincibleAfterHit() const {
    return invincibleAfterHit;
}

void GameObject::playSound(const char *path, const int volume) {
    sound = Mix_LoadWAV(path);
    Mix_VolumeChunk(sound, volume);
    Mix_PlayChannel(-1, sound, 0);
}

void GameObject::setPosition(const Vector2D viewPortPos) {
    viewPortPosition = viewPortPos;
    screenPosition = worldToScreenPos(viewPortPos);
}

void GameObject::init(const int hp, const int _attackDamage) {
    healthPoints = hp;
    attackDamage = _attackDamage;
    init();
}

void GameObject::update(float dt, const Vector2D &viewPortPos, const int initialHP, const int _attackDamage) {
    if (destroyedForever) return;
    setPosition(viewPortPos);
    if (!enabled && screenPosition.x + spriteDims.x >= 0 && screenPosition.x <= gameWidth)
        init(initialHP, _attackDamage);
    GameObject::update(dt, viewPortPos);
}

void GameObject::create(unsigned int _gameWidth, Tag _type, Mode _mode, const Vector2D worldPos) {
    gameWidth = _gameWidth;
    type = _type;
    mode = _mode;
    worldPosition = worldPos;
}

bool GameObject::isGameOver() const {
    return gameOver;
}

bool GameObject::isLevelWon() const {
    return levelWon;
}
