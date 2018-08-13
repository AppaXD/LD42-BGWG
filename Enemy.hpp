#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Clock.hpp>

class Enemy
{
public:
	Enemy::Enemy(int enemyHealth, int enemyDamage, int enemyDefense, int enemyFireRate, sf::Sprite enemySprite);
	sf::Sprite sprite;
	sf::Clock clock;
	float shootDelay;
	int health, damage, defense;
	bool canShoot();
	bool firstShot = true;
};