#pragma once

#include <SFML/System/Clock.hpp>

class Gun
{
private:
	float shootDelay, damage;
	int cost;
	bool automatic;
	sf::Clock clock;
public:
	Gun::Gun(float roundsPerSecond, float gunDamage, int gunCost, bool automaticWeapon);
	bool isAutomatic();
	bool canShoot();
	void restartClock();
	int getCost();
	float getShootDelay();
	float getDamage();
};