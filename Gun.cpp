#include "Gun.hpp"

Gun::Gun(float roundsPerSecond, float gunDamage, int gunCost, bool automaticWeapon)
{
	shootDelay = 1 / roundsPerSecond;
	damage = gunDamage;
	cost = gunCost;
	automatic = automaticWeapon;
}

bool Gun::isAutomatic()
{
	return automatic;
}

bool Gun::canShoot()
{
	return clock.getElapsedTime().asSeconds() >= shootDelay;
}

void Gun::restartClock()
{
	clock.restart();
}

int Gun::getCost()
{
	return cost;
}

float Gun::getShootDelay()
{
	return shootDelay;
}

float Gun::getDamage()
{
	return damage;
}