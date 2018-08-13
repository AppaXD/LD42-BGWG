#include "Bullet.hpp"

Bullet::Bullet(int bulletRotation, int bulletDamage, bool isPlayers, sf::Sprite bulletSprite)
{
	rotation = bulletRotation;
	playerBullet = isPlayers;
	sprite = bulletSprite;
	damage = bulletDamage;
}

int Bullet::getRotation()
{
	return rotation;
}

int Bullet::getDamage()
{
	return damage;
}

bool Bullet::isPlayers()
{
	return playerBullet;
}