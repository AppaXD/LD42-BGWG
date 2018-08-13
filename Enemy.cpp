#include "Enemy.hpp";

Enemy::Enemy(int enemyHealth, int enemyDamage, int enemyDefense, int enemyFireRate, sf::Sprite enemySprite)
{
	health = enemyHealth;
	damage = enemyDamage;
	defense = enemyDefense;
	shootDelay = (float)1/(float)2;
	sprite = enemySprite;
}

bool Enemy::canShoot()
{
	if (clock.getElapsedTime().asSeconds() >= shootDelay)
	{
		clock.restart();
		return true;
	} 
	return false;
}