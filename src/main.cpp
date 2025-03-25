#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>


class Player
{
private:
    sf::RectangleShape shape;
    float speed;

public:
    Player(float startX, float startY)
    {
        shape.setSize(sf::Vector2f(50, 50));
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(sf::Vector2f(startX, startY));
        speed = 400.0f;
    }

    void move(float offsetX)
    {
        shape.move(sf::Vector2f(offsetX, 0));
    }

    void update(float deltaTime, float windowWidth)
    {
        // Handle keyboard input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && shape.getPosition().x > 0)
        {
            move(-speed * deltaTime);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && shape.getPosition().x + shape.getSize().x <
            windowWidth)
        {
            move(speed * deltaTime);
        }
    }

    sf::FloatRect getBounds() const
    {
        return shape.getGlobalBounds();
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(shape);
    }
};

class Item
{
private:
    sf::CircleShape shape;
    float speed;

public:
    Item(float startX)
    {
        shape.setRadius(15.0f);
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(sf::Vector2f(startX, -30.0f));
        speed = 200.0f + static_cast<float>(rand() % 200);
    }

    void update(float deltaTime)
    {
        shape.move(sf::Vector2f(0, speed * deltaTime));
    }

    bool isOffScreen(float windowHeight) const
    {
        return shape.getPosition().y > windowHeight;
    }

    sf::FloatRect getBounds() const
    {
        return shape.getGlobalBounds();
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(shape);
    }
};

int main()
{
    // Initialize random seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Create window
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600)), "Catch the Falling Objects");
    window.setFramerateLimit(60);

    // Create player
    Player player(375.0f, 520.0f);

    // Game variables
    std::vector<Item> items;
    float itemSpawnTimer = 0.0f;
    float itemSpawnInterval = 1.0f;
    int score = 0;
    int misses = 0;

    // Font and text
    sf::Font font;
    if (!font.openFromFile("arial.ttf"))
    {
        // Handle font loading error
        return -1;
    }

    sf::Text scoreText(font, "", 24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(sf::Vector2f(10.0f, 10.0f));

    sf::Text missesText(font, "", 24);
    missesText.setFillColor(sf::Color::White);
    missesText.setPosition(sf::Vector2f(10.0f, 40.0f));

    // Clock for timing
    sf::Clock clock;

    // Game loop
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

        // Handle events
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        // Update game logic
        player.update(deltaTime, 800.0f);

        // Spawn new items
        itemSpawnTimer += deltaTime;
        if (itemSpawnTimer >= itemSpawnInterval)
        {
            itemSpawnTimer = 0.0f;
            float randomX = static_cast<float>(rand() % 770);
            items.push_back(Item(randomX));

            // Gradually increase difficulty
            itemSpawnInterval = std::max(0.2f, itemSpawnInterval - 0.005f);
        }

        // Update items and check collisions
        for (size_t i = 0; i < items.size(); i++)
        {
            items[i].update(deltaTime);

            // Check for collision with player
            if (items[i].getBounds().findIntersection(player.getBounds()))
            {
                score++;
                items.erase(items.begin() + i);
                i--;
                continue;
            }

            // Check if item is off screen
            if (items[i].isOffScreen(600.0f))
            {
                misses++;
                items.erase(items.begin() + i);
                i--;
            }
        }

        // Update text
        scoreText.setString("Score: " + std::to_string(score));
        missesText.setString("Misses: " + std::to_string(misses));

        // Check for game over (5 misses)
        if (misses >= 5)
        {
            sf::Text gameOverText(font, "Game Over! Final Score: " + std::to_string(score), 32);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition(sf::Vector2f(200.0f, 250.0f));

            window.clear();
            window.draw(gameOverText);
            window.display();

            sf::sleep(sf::seconds(3));
            window.close();
        }

        // Render
        window.clear(sf::Color(50, 50, 50));

        // Draw everything
        player.draw(window);
        for (auto &item: items)
        {
            item.draw(window);
        }
        window.draw(scoreText);
        window.draw(missesText);

        window.display();
    }

    return 0;
}
