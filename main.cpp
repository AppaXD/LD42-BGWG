#include <TGUI/TGUI.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include "Gun.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"

int width = 1216;
int height = 768;

float pi = 3.14159265;

int enemyFirerate = 1.5;

std::string assetsDir = "assets/";

sf::Vector2f centreOf(sf::Sprite sprite)
{
	return sf::Vector2f(sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height / 2);
}

sf::Sprite loadFromTexture(sf::Texture& texture, bool centre = true)
{
	sf::Sprite sprite;
	sprite.setTexture(texture);
	if(centre) sprite.setOrigin(centreOf(sprite));
	return sprite;
}

auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
std::mt19937 gen;

bool shopMenuOpen = false;
bool dead = false;
int money = 0;

int rand(int min, int max)
{
	std::uniform_int_distribution<int> range(min, max);
	return range(gen);
}


sf::Text moneyText;
Gun gun(6, 5, 0, false);
int damageReduction = 0;

void buy(int id)
{
	if (shopMenuOpen)
	{
		switch (id)
		{
			case 0:
				if (money >= 400)
				{
					gun = Gun(4, 12, 400, false);
					money -= 400;
					moneyText.setString("$" + std::to_string(money));
				}
				break;
			case 1:
				if (money >= 1350)
				{
					gun = Gun(7, 12, 1350, true);
					money -= 1350;
					moneyText.setString("$" + std::to_string(money));
				}
			case 2:
				if (money >= 350)
				{
					damageReduction = 2;
					money -= 350;
					moneyText.setString("$" + std::to_string(money));
				}
			case 3: 
				if (money >= 950)
				{
					damageReduction = 4;
					money -= 950;
					moneyText.setString("$" + std::to_string(money));
				}
		}
	}
}

std::vector<Bullet> bullets;
std::vector<Bullet> playerBullets;
std::vector<sf::Sprite> level;
std::vector<Enemy> enemies;

int tileSize = 64;

int levels[12] = {
	12, 13, 14,
	16, 18, 18,
	20, 21, 22,
	23, 24, 32 // meant to lose on level 12
};

int currentLevel = 0;
float health = 100;

int main()
{
	sf::RenderWindow window(sf::VideoMode(width, height), "Ludum Dare 42");
	window.setFramerateLimit(90);

	tgui::Gui gui{ window };

	sf::Clock deltaClock;
	sf::Clock spawnClock;
	sf::Clock timerClock;

	sf::Texture t_tileGrass;
	t_tileGrass.loadFromFile(assetsDir + "Tiles/tile_01.png");
	sf::Sprite grass = loadFromTexture(t_tileGrass, false);

	sf::Texture t_roadLeft;
	t_roadLeft.loadFromFile(assetsDir + "Tiles/tile_28.png");
	sf::Sprite roadLeft = loadFromTexture(t_roadLeft, false);

	sf::Texture t_roadRight;
	t_roadRight.loadFromFile(assetsDir + "Tiles/tile_29.png");
	sf::Sprite roadRight = loadFromTexture(t_roadRight, false);

	sf::Texture t_road;
	t_road.loadFromFile(assetsDir + "Tiles/tile_93.png");
	sf::Sprite road = loadFromTexture(t_road, false);

	sf::Texture t_roadMid;
	t_roadMid.loadFromFile(assetsDir + "Tiles/tile_55.png");
	sf::Sprite roadMid = loadFromTexture(t_roadMid, false);

	sf::Texture t_zombie;
	t_zombie.loadFromFile(assetsDir + "Zombie 1/zoimbie1_gun.png");
	sf::Sprite zombie = loadFromTexture(t_zombie);

	sf::Texture t_hitman;
	t_hitman.loadFromFile(assetsDir + "Hitman 1/hitman1_gun.png");
	sf::Sprite hitman = loadFromTexture(t_hitman);

	sf::Texture t_blueMan;
	t_blueMan.loadFromFile(assetsDir + "Man Blue/manBlue_gun.png");
	sf::Sprite blueMan = loadFromTexture(t_blueMan);

	sf::Texture t_brownMan;
	t_brownMan.loadFromFile(assetsDir + "Man Brown/manBrown_gun.png");
	sf::Sprite brownMan = loadFromTexture(t_brownMan);

	sf::Texture t_robot;
	t_robot.loadFromFile(assetsDir + "Robot 1/robot1_gun.png");
	sf::Sprite robot = loadFromTexture(t_robot);

	sf::Texture t_oldMan;
	t_oldMan.loadFromFile(assetsDir + "Man Old/manOld_gun.png");
	sf::Sprite oldMan = loadFromTexture(t_oldMan);

	std::vector<Enemy> enemyLevels = {
		Enemy(10, 6, 0, enemyFirerate, zombie),
		Enemy(12, 7, 0, enemyFirerate, hitman),
		Enemy(12, 8, 0, enemyFirerate, blueMan),
		Enemy(15, 10, 0, enemyFirerate, brownMan),
		Enemy(16, 12, 0, enemyFirerate, robot),
		Enemy(2000, 50, 0, enemyFirerate, oldMan)
	};

	std::vector<int> enemyLevelsIndex = {
		0,
		1, 1,
		2, 2,
		3, 3,
		4, 4, 4, 4,
		5
	};

	std::ifstream levelFile("level.txt");
	std::string line;
	std::vector<std::string> levelData;

	if (levelFile.is_open())
	{
		while (getline(levelFile, line))
		{
			levelData.push_back(line);
		}
	}

	int count = 0;
	for (int i = 0; i < levelData.size(); i++)
	{
		for (int j = 0; j < levelData[i].size(); j++)
		{
			switch (levelData[i][j])
			{
				case '0':
					grass.setPosition(j*tileSize, i*tileSize);
					level.push_back(grass);
					break;
				case '1':
					roadLeft.setPosition(j*tileSize, i*tileSize);
					level.push_back(roadLeft);
					break;
				case '2':
					roadRight.setPosition(j*tileSize, i*tileSize);
					level.push_back(roadRight);
					break;
				case '3':
					road.setPosition(j*tileSize, i*tileSize);
					level.push_back(road);
					break;
				case '4':
					roadMid.setPosition(j*tileSize, i*tileSize);
					level.push_back(roadMid);
					break;
			}
		}
	}

	sf::Font comicSans;
	comicSans.loadFromFile("comicSans.ttf");
	sf::Text timer;
	timer.setCharacterSize(25);
	timer.setFont(comicSans);
	timer.setString("20");

	sf::FloatRect timerRect = timer.getLocalBounds();
	timer.setPosition(sf::Vector2f(width - timerRect.width - 6, 35));

	sf::Text healthText = timer;
	healthText.setPosition(6, 35);
	healthText.setString("100");

	moneyText = timer;
	moneyText.setPosition(305, 0);
	moneyText.setString("$0");

	sf::Text deathText = timer;
	deathText.setString("		    You died.\nPress space to try again.");
	sf::FloatRect textRect = deathText.getLocalBounds();
	deathText.setPosition(width / 2 - (textRect.width/2), height / 2 - (textRect.height/2));

	sf::Text roundText = timer;
	roundText.setString("Round 1");
	roundText.setPosition(795, 0);

	sf::RectangleShape blackBox;
	blackBox.setSize(sf::Vector2f(width, height));
	blackBox.setFillColor(sf::Color::Black);

	float deltaTime = 0.0;
	int seconds = 0;

	sf::RectangleShape timerBar;
	sf::RectangleShape healthBar;

	healthBar.setSize(sf::Vector2f(300, 30));
	healthBar.move(0, 2);
	healthBar.setOutlineThickness(2);
	healthBar.setOutlineColor(sf::Color::Black);

	timerBar.setPosition(sf::Vector2f(width - 300, 0));
	timerBar.setSize(sf::Vector2f(300, 30));
	timerBar.move(0, 2);
	timerBar.setOutlineThickness(2);
	timerBar.setOutlineColor(sf::Color::Black);

	sf::RectangleShape timerBarFill;
	sf::RectangleShape healthBarFill;

	healthBarFill.setSize(sf::Vector2f(300, 30));
	healthBarFill.move(0, 2);
	healthBarFill.setFillColor(sf::Color::Red);

	timerBarFill.setPosition(sf::Vector2f(width - 300, 0));
	timerBarFill.setSize(sf::Vector2f(300, 30));
	timerBarFill.move(0, 2);
	timerBarFill.setFillColor(sf::Color::Blue);


	sf::Texture t_player;
	t_player.loadFromFile(assetsDir + "Survivor 1/survivor1_gun.png");
	sf::Sprite player = loadFromTexture(t_player);
	player.setPosition(width / 2, height / 2);

	sf::Texture t_bullet;
	t_bullet.loadFromFile(assetsDir + "bullet.png");
	sf::Sprite bullet = loadFromTexture(t_bullet);
	bullet.setScale(0.004, 0.004);
	bullet.setPosition(100, 100);

	tgui::Theme babyBlue;
	babyBlue.load("BabyBlue.txt");

	sf::RectangleShape transparentBg;
	transparentBg.setFillColor(sf::Color(192, 192, 192, 128));
	transparentBg.setSize(sf::Vector2f(width, height));

	tgui::Button::Ptr buyTemplate = tgui::Button::create();
	buyTemplate->setRenderer(babyBlue.getRenderer("button"));
	buyTemplate->setSize("250", "55");
	buyTemplate->setPosition("-500", "-500");
	buyTemplate->setTextSize(16);
	
	tgui::Button::Ptr deagle = tgui::Button::copy(buyTemplate);
	deagle->setText("Desert Eagle - $400");
	deagle->setPosition("4%", "12%");
	deagle->connect("pressed", buy, 0);
	gui.add(deagle);

	tgui::Button::Ptr ak47 = tgui::Button::copy(buyTemplate);
	ak47->setText("Ak47 - $1350");
	ak47->setPosition("4%", "21%");
	ak47->connect("pressed", buy, 1);
	gui.add(ak47);

	tgui::Button::Ptr lightArmour = tgui::Button::copy(buyTemplate);
	lightArmour->setText("Light Armour - $350");
	lightArmour->setPosition("4%", "30%");
	lightArmour->connect("pressed", buy, 2);
	gui.add(lightArmour);

	tgui::Button::Ptr heavyArmour = tgui::Button::copy(buyTemplate);
	heavyArmour->setText("Heavy Armour - $950");
	heavyArmour->setPosition("4%", "39%");
	heavyArmour->connect("pressed", buy, 3);
	gui.add(heavyArmour);

	tgui::Button::Ptr nextLevel = tgui::Button::copy(buyTemplate);
	nextLevel->setText("Next Level");
	nextLevel->setPosition("4%", "48%");
	nextLevel->connect("pressed", [&] { if (shopMenuOpen) { shopMenuOpen = false; timerClock.restart(); }});
	gui.add(nextLevel);

	gui.remove(buyTemplate);

	sf::SoundBuffer gunShotBuffer;
	gunShotBuffer.loadFromFile(assetsDir + "shot.ogg");

	sf::Sound gunShot;
	gunShot.setBuffer(gunShotBuffer);
	gunShot.setVolume(8);


	int acceleration = 0;

	gun.restartClock();

	float spawnrate = (float)20 / (float)levels[currentLevel];
	float timeLeft = 20.0;

	bool mouseDown = false;

	while (window.isOpen())
	{
		deltaTime = deltaClock.restart().asSeconds();

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
			{
				bool r = event.type == sf::Event::KeyReleased;
				int key = event.key.code;
				switch (key)
				{
					case sf::Keyboard::W:
						acceleration = r ? 0 : 150;
						break;
					case sf::Keyboard::S:
						acceleration = r ? 0 : -150;
						break;
					case sf::Keyboard::Space:
						if (dead)
						{
							currentLevel = 0;
							health = 100;
							enemies.clear();
							timerClock.restart();
							moneyText.setString("$0");
							healthText.setString("100");
							roundText.setString("Round 1");
							dead = false;
							shopMenuOpen = false;
							player.setPosition(width / 2, height / 2);
							gun = Gun(6, 5, 0, false);
							money = 0;
						}
				}
			}
			else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && !shopMenuOpen)
			{
				mouseDown = true;

				if (gun.canShoot())
				{
					gun.restartClock();

					sf::Vector2f pos = player.getPosition();

					int angle = player.getRotation();
					Bullet bullet(angle, gun.getDamage(), true, bullet);
					bullet.sprite.setRotation(180 + player.getRotation());
					bullet.sprite.setPosition(pos.x + cos(angle), pos.y + sin(angle));
					playerBullets.push_back(bullet);

					gunShot.play();
				}
			}
			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
			{
				mouseDown = false;
			}

			gui.handleEvent(event);
		}

		if (!shopMenuOpen)
		{
			if (mouseDown && gun.isAutomatic())
			{
				if (gun.canShoot())
				{
					gun.restartClock();

					sf::Vector2f pos = player.getPosition();

					int angle = player.getRotation();
					Bullet bullet(angle, gun.getDamage(), true, bullet);
					bullet.sprite.setRotation(180 + player.getRotation());
					bullet.sprite.setPosition(pos.x + cos(angle), pos.y + sin(angle));
					playerBullets.push_back(bullet);

					gunShot.play();
				}
			}

			if (spawnClock.getElapsedTime().asSeconds() >= spawnrate)
			{
				spawnClock.restart();

				int p = rand(1, 4);

				int x, y;

				switch (p)
				{
				case 1:
					x = -25;
					y = rand(0, height);
					break;
				case 2:
					x = rand(0, width);
					y = 0;
					break;
				case 3:
					x = width + 25;
					y = rand(0, height);
					break;
				case 4:
					x = rand(0, width);
					y = height + 25;
					break;
				}

				Enemy enemy = enemyLevels[rand(0, enemyLevelsIndex[currentLevel])];
				enemy.sprite.setPosition(x, y);
				enemies.push_back(enemy);
			}

			sf::Vector2f playerPos = player.getPosition();

			sf::Vector2i mousePos = sf::Mouse::getPosition(window);

			float dx = playerPos.x - mousePos.x;
			float dy = playerPos.y - mousePos.y;

			float angle = 180 + ((atan2(dy, dx)) * 180 / pi);

			player.setRotation(angle);

			player.move(deltaTime * acceleration * cos(angle * pi / 180), deltaTime * acceleration * sin(angle * pi / 180));

			timeLeft = 20.0 - timerClock.getElapsedTime().asSeconds();
			healthBarFill.setSize(sf::Vector2f(healthBar.getSize().x / (100 / health), 30));
			timerBarFill.setSize(sf::Vector2f(timerBar.getSize().x / (20 / timeLeft), 30));

			if (timeLeft <= 0.0)
			{
				timerClock.restart();

				currentLevel++;
				roundText.setString("Round " + std::to_string(currentLevel+1));
				health = 100;
				healthText.setString("100");
				spawnrate = 20.0 / levels[currentLevel];
				money += 400;
				moneyText.setString("$" + std::to_string(money));
				shopMenuOpen = true;
				enemies.clear();
			}

			timer.setString(std::to_string((int)timeLeft));

			window.clear();

			for (int i = 0; i < level.size(); i++)
			{
				window.draw(level[i]);
			}

			for (int i = 0; i < bullets.size(); i++)
			{
				int bulletAngle = bullets[i].getRotation();
				bullets[i].sprite.move(deltaTime * 3000 * cos(bulletAngle * pi / 180), deltaTime * 3000 * sin(bulletAngle * pi / 180));
				window.draw(bullets[i].sprite);

				if (bullets[i].sprite.getGlobalBounds().intersects(player.getGlobalBounds()))
				{
					float damage = (float)bullets[i].getDamage();
					damage = damage - (damage / 10.0 * damageReduction);
					health -= damage;
					healthText.setString(std::to_string((int)health));
					bullets.erase(bullets.begin() + i);
					if (health <= 0)
					{
						dead = true;
					}
				}
			}

			for (int i = 0; i < playerBullets.size(); i++)
			{
				int bulletAngle = playerBullets[i].getRotation();
				playerBullets[i].sprite.move(deltaTime * 3000 * cos(bulletAngle * pi / 180), deltaTime * 3000 * sin(bulletAngle * pi / 180));
				window.draw(playerBullets[i].sprite);
			}

			for (int i = 0; i < enemies.size(); i++)
			{
				sf::FloatRect bounds = enemies[i].sprite.getGlobalBounds();
				for (int j = 0; j < playerBullets.size(); j++)
				{
					if (playerBullets[j].sprite.getGlobalBounds().intersects(bounds))
					{
						enemies[i].health -= playerBullets[j].getDamage();
						playerBullets.erase(playerBullets.begin() + j);
					}
				}

				sf::Vector2f enemyPos = enemies[i].sprite.getPosition();

				float dx = playerPos.x - enemyPos.x;
				float dy = playerPos.y - enemyPos.y;

				float enemyAngle = atan2(dy, dx) * 180 / pi;

				int distance = sqrt(pow(enemyPos.x - playerPos.x, 2) + pow(enemyPos.y - playerPos.y, 2));

				enemies[i].sprite.setRotation(enemyAngle);

				if (distance > 110)
					enemies[i].sprite.move(deltaTime * 90 * cos(enemyAngle * pi / 180), deltaTime * 90 * sin(enemyAngle * pi / 180));

				if (enemies[i].canShoot())
				{
					enemies[i].clock.restart();

					int accuracy = floor(distance / 4);

					if (enemies[i].firstShot)
					{
						accuracy * 1.5;
					}

					int x = playerPos.x + rand(-50 - accuracy, 50 + accuracy);
					int y = playerPos.y + rand(-50 - accuracy, 50 + accuracy);

					float shotAngle = atan2(y - enemyPos.y, x - enemyPos.x) * 180 / pi;

					Bullet bullet(shotAngle, enemies[i].damage, false, bullet);
					bullet.sprite.setRotation(180 + shotAngle);
					bullet.sprite.setPosition(enemyPos.x + cos(shotAngle), enemyPos.y + sin(shotAngle));
					bullets.push_back(bullet);
					gunShot.play();
				}

				window.draw(enemies[i].sprite);

				if (enemies[i].health <= 0)
					enemies.erase(enemies.begin() + i);
			}
		}

		if (shopMenuOpen) {
			window.clear();
			for (int i = 0; i < level.size(); i++)
			{
				window.draw(level[i]);
			}
		}

		window.draw(player);
		window.draw(healthBar);
		window.draw(healthBarFill);
		window.draw(timerBar);
		window.draw(timerBarFill);
		window.draw(timer);
		window.draw(healthText);
		window.draw(moneyText);
		window.draw(roundText);
		if (shopMenuOpen)
		{
			window.draw(transparentBg);
			gui.draw();
		}

		if (dead)
		{
			window.draw(blackBox);
			window.draw(deathText);
		}
	
		window.display();
	}

	return 0;
}
