#include <SFML/Graphics.hpp>
#include <vector>

int main() {
	// Define window
	const int w = 800;
	const int h = 600;
	sf::RenderWindow window(sf::VideoMode({ w, h }), "Pong");
	
	// Set framerate limit
	window.setFramerateLimit(60);

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
	sf::Vector2f ballVelocity({-5.f, 3.f});

	bool gameStarted = false;

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
			}
		}
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

		// Ball movement
		if (gameStarted) {
			ball.move(ballVelocity);

			// Bounce off top and bottom walls
			if (ball.getPosition().y <= 0 || ball.getPosition().y + ball.getRadius() * 2 >= 600)
				ballVelocity.y = -ballVelocity.y;

			// Bounce off paddles
			if (ball.getGlobalBounds().findIntersection(player.getGlobalBounds()) ||
				ball.getGlobalBounds().findIntersection(ai.getGlobalBounds())) {
				ballVelocity.x = -ballVelocity.x;
			}

			if (ball.getPosition().x < 0 || ball.getPosition().x > 800) {
				// Restart
				ball.setPosition({ 400.f, 300.f });
				ballVelocity = { -5.f, -3.f };
			}
		}

		// Clear window
		window.clear(sf::Color(14, 16, 18));

		// Draw shapes
		window.draw(player);
		window.draw(ai);
		window.draw(ball);

		for (auto& segment : centerLine) {
			window.draw(segment);
		}

		// Display
		window.display();
	}

	return 0;
}