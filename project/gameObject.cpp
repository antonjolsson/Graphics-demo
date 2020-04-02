
#include "avancezlib.h"
#include "gameObject.h"


#include "../external/SDL2_mixer/include/SDL_mixer.h"
#include "behaviour.h"

void GameObject::create() {
	
}

void GameObject::addComponent(Component* _component) {
	components.push_back(_component);
}

void GameObject::addComponents(const std::vector<Component*>& _components) {
    for (auto comp : _components)
        addComponent(comp);
}

void GameObject::init() {
	for (auto& component : components)
        component->init();
    setEnabled(true);
}

void GameObject::update(const float _dt) {
	if (!isEnabled()) return;
	for (auto& component : components) {
        component->update(_dt);
	}
}

void GameObject::destroy() {
	for (auto& component : components)
        component->destroy();
}

GameObject::~GameObject() = default;

void GameObject::addReceiver(GameObject* _go) {
	receivers.push_back(_go);
}

void GameObject::send(Message _m) {
	for (auto& receiver : receivers) {
		if (!receiver->isEnabled())
			continue;
        receiver->receive(_m);
	}
}

void GameObject::receive(Message _m) {
    if (!isEnabled()) return;
    switch (_m) {
        case MOVED_LEFT: case MOVED_RIGHT:
            sendComponents(_m);
        case NO_INPUT:
            sendComponents(_m);
            break;
        case FIRE:
            sendComponents(_m);
            break;
        case BULLET_HIT: case PLAYER_HIT:
            sendComponents(_m);
            break;
        case OBJ_IN_AIR:
            mode = IN_AIR;
            sendComponents(_m);
            break;
        case GAME_OVER:
            gameOver = true;
            sendComponents(_m);
            break;
        case LEVEL_WON:
            levelWon = true;
            break;
        default: break;
    }
}

void GameObject::sendComponents(Message _message) {
    for (auto& component : components)
        component->receive(_message);
}

void GameObject::create(unsigned int _gameWidth) {
    
}

int GameObject::getAttackDamage() {
	const Behaviour behaviour = *getComponent<Behaviour>();
    return behaviour.getAttackDamage();
}

bool GameObject::isInvincibleAfterHit() {
    const Behaviour behaviour = *getComponent<Behaviour>();
    return behaviour.isInvincibleAfterHit();
}

void GameObject::playSound(const char *_path, const int _volume) {
    sound = Mix_LoadWAV(_path);
    Mix_VolumeChunk(sound, _volume);
    Mix_PlayChannel(-1, sound, 0);
}

void GameObject::init(const int hp, const int _attackDamage) {
    init();
}

void GameObject::create(const Tag _type, const Mode _mode, const glm::vec3 _position) {
    type = _type;
    mode = _mode;
    position = _position;
}

void GameObject::setEnabled(const bool _enabled)
{
	enabled = _enabled;
}

bool GameObject::isEnabled() const
{
	return enabled;
}

bool GameObject::isGameOver() const {
    return gameOver;
}

bool GameObject::isLevelWon() const {
    return levelWon;
}
