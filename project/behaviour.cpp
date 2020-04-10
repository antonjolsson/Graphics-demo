#include "behaviour.h"

int Behaviour::getAttackDamage() const
{
	return attackDamage_;
}

bool Behaviour::isInvincibleAfterHit() const
{
	return invincibleAfterHit_;
}

void Behaviour::update(float _dt)
{
	
}

Behaviour::Behaviour(GameObject* _go, Engine* _engine) {
	go = _go;
	engine = _engine;
}
