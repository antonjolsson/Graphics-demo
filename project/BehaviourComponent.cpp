#include "BehaviourComponent.h"

int BehaviourComponent::getAttackDamage() const
{
	return attackDamage_;
}

bool BehaviourComponent::isInvincibleAfterHit() const
{
	return invincibleAfterHit_;
}

void BehaviourComponent::update(float _dt)
{
	
}

BehaviourComponent::BehaviourComponent(GameObject* _go, InputHandler* _engine) {
	go = _go;
	inputHandler = _engine;
}
