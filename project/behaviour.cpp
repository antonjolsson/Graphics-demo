#include "behaviour.h"

int Behaviour::getAttackDamage() const
{
	return attackDamage_;
}

bool Behaviour::isInvincibleAfterHit() const
{
	return invincibleAfterHit_;
}

void Behaviour::update(float dt)
{
	
}
