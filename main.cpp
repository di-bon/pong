#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>

#define WIDTH 1280
#define HEIGHT 1024

#define PLAYER_WIDTH 16
#define PLAYER_HEIGHT 128

#define BALL_WIDTH 16
#define BALL_HEIGHT 16
#define BALL_SPEED 16

#define PLAYER_VERTICAL_SPEED 16

void handlePlayerInput(sf::RectangleShape &player, const int increment, const float increment_multiplier);
void normaliseV2f(sf::Vector2f &vector);
bool isColliding(sf::RectangleShape &b, sf::RectangleShape &player, bool isRightPlayer);
void resetBallPositionAndDirection(sf::RectangleShape &ball, int &x_direction, int &y_direction);
void updateTextScore(sf::Text &text, int leftPlayerScore, int rightPlayerScore);
bool isButtonPressed(sf::RectangleShape &button, sf::Vector2i position, int outlineThickness);
void positionMusicButtonAndText(sf::RectangleShape &button, sf::Text &text, sf::Text &upperElement);

int main()
{
    srand(time(NULL));

    int x_direction = 1;
    int y_direction = 1;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Dib pong");
    window.setFramerateLimit(30);

    sf::RectangleShape leftPlayer;
    sf::RectangleShape rightPlayer;
    sf::RectangleShape ball;

    ball.setSize(sf::Vector2f(BALL_WIDTH, BALL_HEIGHT));
    resetBallPositionAndDirection(ball, x_direction, y_direction);

    sf::Vector2f ballDirection;

    ballDirection = sf::Vector2f(3, 4);
    normaliseV2f(ballDirection);
    std::cout << "Ball direction: " << ballDirection.x << ", " << ballDirection.y << std::endl;

    sf::Vector2f playerDimension(PLAYER_WIDTH, PLAYER_HEIGHT);

    leftPlayer.setSize(playerDimension);
    rightPlayer.setSize(playerDimension);

    leftPlayer.setPosition(0, HEIGHT / 2 - (leftPlayer.getSize().y / 2));
    rightPlayer.setPosition(WIDTH - rightPlayer.getSize().x, HEIGHT / 2 - (rightPlayer.getSize().y / 2));

    int leftPlayerScore = 0;
    int rightPlayerScore = 0;

    std::string fontPath = "fonts/arcade-text-font.ttf";
    sf::Font font;
    if (!font.loadFromFile(fontPath))
    {
        std::cerr << "Error: cannot find " << fontPath << std::endl;
        exit(1);
    }

    sf::Text textScore;
    textScore.setFont(font);
    updateTextScore(textScore, leftPlayerScore, rightPlayerScore);
    textScore.setPosition(WIDTH / 2 - textScore.getLocalBounds().width / 2, 10);

    std::string x_ballSoundPath = "sfx/x-bouncing-sound.wav";
    std::string y_ballSoundPath = "sfx/y-bouncing-sound.wav";

    sf::SoundBuffer ballSoundBuffer;
    sf::Sound ballSound;
    ballSound.setBuffer(ballSoundBuffer);

    sf::Music music;
    std::string soundstrackPath = "sfx/soundtrack.wav";
    if (!music.openFromFile(soundstrackPath))
    {
        std::cout << "Error: cannot find " << soundstrackPath << std::endl;
    }
    music.setLoop(true);
    music.play();

    // button music_button = button(16, 16, 32, 32);
    // button music_button;
    // music_button.setX(16);
    // music_button.setY(16);
    // music_button.setWidth(32);
    // music_button.setHeight(32);
    // music_button.initialise();

    sf::RectangleShape musicButton;
    musicButton.setPosition(16, 16);
    musicButton.setSize(sf::Vector2f(32, 32));
    musicButton.setOutlineThickness(4);
    musicButton.setFillColor(sf::Color::Transparent);
    musicButton.setOutlineColor(sf::Color::White);

    sf::Text textMusicButton;
    textMusicButton.setFont(font);
    std::string disableMusic = "Disable music";
    std::string enableMusic = "Enable music";
    textMusicButton.setString(disableMusic);
    positionMusicButtonAndText(musicButton, textMusicButton, textScore);

    float previousMusicVolume = 5.0f;

    sf::Vector2i mousePosition;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    mousePosition = sf::Mouse::getPosition(window);
                    if (isButtonPressed(musicButton, mousePosition, musicButton.getOutlineThickness()))
                    {
                        std::cout << "Button pressed" << std::endl;
                        if (music.getVolume() == 0.0f)
                        {
                            music.setVolume(previousMusicVolume);
                            textMusicButton.setString(disableMusic);
                        }
                        else
                        {
                            previousMusicVolume = music.getVolume();
                            music.setVolume(0.0f);
                            textMusicButton.setString(enableMusic);
                        }
                        positionMusicButtonAndText(musicButton, textMusicButton, textScore);
                    }
                }
            }
        }

        // if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        // {
        //     mousePosition = sf::Mouse::getPosition(window);
        //     if (isButtonPressed(musicButton, mousePosition))
        //     {
        //         std::cout << "Button pressed" << std::endl;
        //         if (ballSound.getVolume() == 0)
        //         {
        //             ballSound.setVolume(previousMusicVolume);
        //         }
        //         else
        //         {
        //             previousMusicVolume = ballSound.getVolume();
        //             ballSound.setVolume(0);
        //         }
        //     }
        // }

        // left player
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            handlePlayerInput(leftPlayer, -PLAYER_VERTICAL_SPEED, 1);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            handlePlayerInput(leftPlayer, PLAYER_VERTICAL_SPEED, 1);
        }

        // right player
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            handlePlayerInput(rightPlayer, -PLAYER_VERTICAL_SPEED, 1);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            handlePlayerInput(rightPlayer, PLAYER_VERTICAL_SPEED, 1);
        }

        // ball
        ball.setPosition(
            ball.getPosition().x + ballDirection.x * x_direction * BALL_SPEED,
            ball.getPosition().y + ballDirection.y * y_direction * BALL_SPEED);
        if (ball.getPosition().y < 0 || ball.getPosition().y > HEIGHT - BALL_HEIGHT)
        {
            y_direction *= -1;
            if (!ballSoundBuffer.loadFromFile(y_ballSoundPath))
            {
                std::cerr << "Error: cannot find " << y_ballSoundPath << std::endl;
                exit(1);
            }
            ballSound.play();
        }

        if (isColliding(ball, leftPlayer, false) || isColliding(ball, rightPlayer, true))
        {
            x_direction *= -1;
            if (!ballSoundBuffer.loadFromFile(x_ballSoundPath))
            {
                std::cerr << "Error: cannot find " << x_ballSoundPath << std::endl;
                exit(1);
            }
            ballSound.play();
        }

        if (ball.getPosition().x < 0)
        {
            rightPlayerScore++;
            resetBallPositionAndDirection(ball, x_direction, y_direction);
            updateTextScore(textScore, leftPlayerScore, rightPlayerScore);
        }
        else if (ball.getPosition().x > WIDTH)
        {
            leftPlayerScore++;
            resetBallPositionAndDirection(ball, x_direction, y_direction);
            updateTextScore(textScore, leftPlayerScore, rightPlayerScore);
        }

        window.clear(sf::Color::Black);

        window.draw(textScore);
        window.draw(ball);
        window.draw(leftPlayer);
        window.draw(rightPlayer);
        // window.draw(music_button.getShape());
        window.draw(textMusicButton);
        window.draw(musicButton);

        window.display();
    }

    return 0;
}

// TODO: review verticalSpeedMultiplier: decide if delete it or not.

void handlePlayerInput(sf::RectangleShape &player, const int verticalSpeed, const float verticalSpeedMultiplier)
{
    player.setPosition(player.getPosition().x, player.getPosition().y + verticalSpeed * verticalSpeedMultiplier);
    if (player.getPosition().y > (HEIGHT - PLAYER_HEIGHT))
    {
        player.setPosition(player.getPosition().x, HEIGHT - PLAYER_HEIGHT);
    }
    else if (player.getPosition().y < 0)
    {
        player.setPosition(player.getPosition().x, 0);
    }
}

void normaliseV2f(sf::Vector2f &vector)
{
    if (vector == sf::Vector2f(0, 0))
    {
        return;
    }
    float length = sqrt(vector.x * vector.x + vector.y * vector.y);
    float inverseLength = 1.0 / length;
    vector.x = inverseLength * vector.x;
    vector.y = inverseLength * vector.y;
}

bool isColliding(sf::RectangleShape &b, sf::RectangleShape &player, bool isRightPlayer)
{
    bool res = true;
    // int adjustment = isRightPlayer ? BALL_WIDTH : 0;
    res = res && ((b.getPosition().x) - (player.getPosition().x) > 0 && (b.getPosition().x) - (player.getPosition().x) < PLAYER_WIDTH);
    res = res && ((b.getPosition().y) - (player.getPosition().y) > 0 && (b.getPosition().y) - (player.getPosition().y) < PLAYER_HEIGHT);

    return res;
}

void resetBallPositionAndDirection(sf::RectangleShape &ball, int &x_direction, int &y_direction)
{
    ball.setPosition(WIDTH / 2 - (BALL_WIDTH / 2), HEIGHT / 2 - (BALL_HEIGHT / 2));
    x_direction = 1;
    y_direction = 1;
}

void updateTextScore(sf::Text &text, int leftPlayerScore, int rightPlayerScore)
{
    std::ostringstream oss;
    oss << "Score: " << leftPlayerScore << " - " << rightPlayerScore;
    text.setString(oss.str());
    std::cout << oss.str() << std::endl;
}

bool isButtonPressed(sf::RectangleShape &button, sf::Vector2i position, int outlineThickness)
{
    return (position.x >= button.getPosition().x - outlineThickness && position.x <= button.getPosition().x + button.getSize().x + outlineThickness) &&
           (position.y >= button.getPosition().y - outlineThickness && position.y <= button.getPosition().y + button.getSize().y + outlineThickness);
}

void positionMusicButtonAndText(sf::RectangleShape &button, sf::Text &text, sf::Text &upperElement)
{
    int spacingBetweenElements = 16;
    int verticalPadding = 20;
    int height = upperElement.getPosition().y + upperElement.getLocalBounds().height + verticalPadding;
    int totalWidth = button.getSize().x + spacingBetweenElements + text.getLocalBounds().width;
    button.setPosition(
        WIDTH / 2 - totalWidth / 2,
        height);
    text.setPosition(button.getPosition().x + button.getLocalBounds().width + spacingBetweenElements, height);
}