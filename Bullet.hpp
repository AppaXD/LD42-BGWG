#pragma once

#include <SFML/Graphics/Sprite.hpp>

class Bullet
{
private:
	int rotation, damage;
	bool playerBullet;
public:
	Bullet::Bullet(int bulletRotation, int bulletDamage, bool isPlayers, sf::Sprite bulletSprite);
	int getRotation();
	int getDamage();
	bool isPlayers();
	sf::Sprite sprite;
};