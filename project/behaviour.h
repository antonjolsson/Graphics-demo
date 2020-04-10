#pragma once
#include "../../project/component.h"

class Behaviour : public Component
{
	
	int attackDamage_ = 0;
public:
	int getAttackDamage() const;
	bool isInvincibleAfterHit() const;
private:
	bool invincibleAfterHit_ = false;
	int healthPoints_ = 0;

public:

	virtual void decreaseHP(int _damage) {};

	void update(float _dt) override;

	Behaviour(GameObject* _go, Engine* _engine);
};

