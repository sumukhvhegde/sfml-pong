#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <string>

int main() {
	// Define window
	const int w = 800;
	const int h = 600;
	sf::RenderWindow window(sf::VideoMode({ w, h }), "Pong", sf::Style::Titlebar | sf::Style::Close);

	// Set framerate limit
	window.setFramerateLimit(72);

	// Game objects
	sf::RectangleShape player;
	player.setSize({ 20.f, 100.f });
	player.setPosition({ 30.f, 250.f });
	player.setFillColor({ sf::Color::White });

	sf::RectangleShape ai;
	ai.setSize({ 20.f, 100.f });
	ai.setPosition({ 750.f, 250.f });

	sf::CircleShape ball(10.f);
	ball.setFillColor(sf::Color::White);
	ball.setPosition({ 400.f, 300.f });

	// Separation line
	std::vector<sf::RectangleShape> centerLine;
	float segementHeight = 20.f;
	float gap = 15.f;
	float x = 800.f / 2.f - 2.f;

	for (float y = 0; y < 600; y += segementHeight + gap) {
		sf::RectangleShape segment({ 4.f, segementHeight });
		segment.setFillColor(sf::Color::White);
		segment.setPosition({ x, y });
		centerLine.push_back(segment);
	}

	// Ball velocity
	sf::Vector2f ballVelocity({ -5.f, 3.f });

	bool gameStarted = false;
	bool isPaused = false;
	bool hasWon = false;

	// Score
	int playerScore = 0;
	int aiScore = 0;

	// Load font
	sf::Font font;
	if (!font.openFromFile("font/arialbd.ttf")) {
		std::cerr << "Error: couldn't load the font!" << std::endl;
		return EXIT_FAILURE;
	}

	// Pause text
	sf::Text pauseText(font);
	pauseText.setString("PAUSED!");
	pauseText.setCharacterSize(50);
	pauseText.setFillColor(sf::Color::White);

	sf::FloatRect bounds = pauseText.getLocalBounds();
	pauseText.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });

	pauseText.setPosition({ w / 2.f, h / 2.f });

	// Player Score Text
	sf::Text playerScoreText(font);
	playerScoreText.setCharacterSize(30);
	playerScoreText.setFillColor(sf::Color::White);
	playerScoreText.setPosition({200.f, 20.f});

	// AI Score Text
	sf::Text aiScoreText(font);
	aiScoreText.setCharacterSize(30);
	aiScoreText.setFillColor(sf::Color::White);
	aiScoreText.setPosition({ 600.f, 20.f });

	// Win Text
	sf::Text winText(font);
	winText.setCharacterSize(50);
	winText.setFillColor(sf::Color::White);
	winText.setPosition({ w / 2.f, h / 2.f });

	auto resetGame = [&](bool fullReset = true) {
		// Reset ball position and velocity
		ball.setPosition({ 400.f, 300.f });
		ballVelocity = { -5.f, -3.f };

		// Reset paddles
		player.setPosition({ 30.f, 250.f });
		ai.setPosition({ 750.f, 250.f });

		if (fullReset) {
			playerScore = 0;
			aiScore = 0;
			hasWon = false;
		}

		gameStarted = false;
		isPaused = false;
	};

	// While window is open
	while (window.isOpen()) {
		while (std::optional event = window.pollEvent()) {
			// Handle event
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			if (auto key = event->getIf<sf::Event::KeyPressed>()) {
				if (key->scancode == sf::Keyboard::Scan::Escape) window.close();
				if (key->scancode == sf::Keyboard::Scan::Space) {
					if (!gameStarted) {
						gameStarted = true;
						ball.setPosition({ 400.f, 300.f });
					}
				}
				if (key->scancode == sf::Keyboard::Scan::P) {
					if (gameStarted) {
						isPaused = !isPaused;
					}
				}
				if (key->scancode == sf::Keyboard::Scan::R) {
					resetGame(true);
				}
			}
		}
		
		// If game is started and not paused
		if (gameStarted && !isPaused) {
			// Player movement
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W) && player.getPosition().y > 0)
				player.move({ 0.f, -6.f });
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S) && player.getPosition().y + player.getSize().y < 600)
				player.move({ 0.f, 6.f });

			// AI movement
			if (ball.getPosition().y + ball.getRadius() < ai.getPosition().y + ai.getSize().y / 2)
				ai.move({ 0.f, -5.f });
			else if (ball.getPosition().y + ball.getRadius() > ai.getPosition().y + ai.getSize().y / 2)
				ai.move({ 0.f, 5.f });

			ball.move(ballVelocity);

			// Bounce off top and bottom walls
			if (ball.getPosition().y <= 0 || ball.getPosition().y + ball.getRadius() * 2 >= 600)
				ballVelocity.y = -ballVelocity.y;

			// Bounce off paddles
			if (ball.getGlobalBounds().findIntersection(player.getGlobalBounds()) ||
				ball.getGlobalBounds().findIntersection(ai.getGlobalBounds())) {
				ballVelocity.x = -ballVelocity.x;
				ballVelocity.x *= 1.4f;
			}
			
			// Cap the speed
			float maxSpeed = 10.f;
			if (std::abs(ballVelocity.x) > maxSpeed)
				ballVelocity.x = (ballVelocity.x > 0 ? maxSpeed : -maxSpeed);
			if (std::abs(ballVelocity.y) > maxSpeed)
				ballVelocity.y = (ballVelocity.y > 0 ? maxSpeed : -maxSpeed);

			if (ball.getPosition().x < 0) {
				// AI scores
				aiScore++;
				ball.setPosition({ 400.f, 300.f });
				ballVelocity = { -5.f, -3.f };
			}

			if (ball.getPosition().x > 800) {
				// Player scores
				playerScore++;
				ball.setPosition({ 400.f, 300.f });
				ballVelocity = { 5.f, -3.f };
			}

			playerScoreText.setString(std::to_string(playerScore));
			aiScoreText.setString(std::to_string(aiScore));

			if (playerScore >= 11) {
				winText.setString("You Win!");
				sf::FloatRect bounds = winText.getLocalBounds();
				winText.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
				winText.setPosition({ w / 2.f, h / 2.f });

				hasWon = true;
				isPaused = true;
				gameStarted = false;
			}

			if (aiScore >= 11) {
				winText.setString("Opponent Wins!");
				sf::FloatRect bounds = winText.getLocalBounds();
				winText.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
				winText.setPosition({ w / 2.f, h / 2.f });

				hasWon = true;
				isPaused = true;
				gameStarted = false;
			}
		}

		// Clear window
		window.clear(sf::Color(14, 16, 18));

		// If game is paused
		if (isPaused && !hasWon) {
			window.draw(pauseText);
		}

		// Draw shapes
		window.draw(player);
		window.draw(ai);
		window.draw(ball);

		window.draw(playerScoreText);
		window.draw(aiScoreText);

		if (hasWon) {
			window.draw(winText);
		}

		for (auto& segment : centerLine) {
			window.draw(segment);
		}

		// Display
		window.display();
	}

	return EXIT_SUCCESS;
}