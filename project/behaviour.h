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

	virtual void decreaseHP(int damage) {};

	void update(float dt) override;
	
};

