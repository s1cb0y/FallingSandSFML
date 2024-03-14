#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <algorithm>

#define SAND_RAD 4
#define GRID_X 100
#define GRID_Y 100
#define WINDOW_X 800
#define WINDOW_Y 800
#define SCAL_FACTOR_X 8
#define SCAL_FACTOR_Y 8

class World{
public:
    World() {}
    ~World(){}

    void Update(sf::RenderWindow& window, const sf::Time& ts){

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed){
                if (sf::Keyboard::Key::R == event.key.code){
                    Reset();
                }
                if (sf::Keyboard::Key::Escape == event.key.code){
                    window.close();
                }
            }
        }

        UpdateSand(window, ts);
        UpdateObstacles(window, ts);

    }

    void UpdateSand(sf::RenderWindow& window, const sf::Time& ts){

        //get current mouse position
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // only create new sandcorn if mouse position is on screen
        if (MouseOnScreen(mousePos) && (sf::Mouse::isButtonPressed(sf::Mouse::Left))){
            // scale screen position to grid position
            m_Grid[mousePos.y / (SCAL_FACTOR_Y)][mousePos.x / (SCAL_FACTOR_X)] = true;
            m_SandCornLifeTime[mousePos.y / (SCAL_FACTOR_Y)][mousePos.x / (SCAL_FACTOR_X)] += ts.asMilliseconds();
        }

        // Update all sandcorns
        for (int x = 0 ; x < GRID_X; x++){
            for (int y = 0 ; y < GRID_Y-1; y++){ // Y-1 to simplify bottom case
                if (m_GridBuffer[y][x] == true){
                    // calculate new position based in s = 0.5* g * t * t
                    float g = 9.81; // m/s2
                    int y_delta =  0.5* g * m_SandCornLifeTimeBuffer[y][x] / 1000.0 * m_SandCornLifeTimeBuffer[y][x] / 1000.0;
                    y_delta= (y_delta < 1) ? 1 : y_delta;
                    int y_new = y + y_delta > GRID_Y-1 ? GRID_Y-1 : y + y_delta;

                    int y_free          = GetNextFreeGridPos(y+1, y_new, x);
                    int y_x_minus1_free = GetNextFreeGridPos(y+1, y_new, x-1);
                    int y_x_plus1_free  = GetNextFreeGridPos(y+1, y_new, x+1);

                    if (y_free > -1){ // space below is free -> just fall
                        m_Grid[y_free][x] = true;
                        m_Grid[y][x]     = false;
                        m_SandCornLifeTime[y_free][x] = m_SandCornLifeTimeBuffer[y][x] + ts.asMilliseconds();
                        m_SandCornLifeTime[y][x]     = 0.0f;
                    }
                    else if ((x-1 >= 0) && (y_x_minus1_free >-1 ) &&
                             (x+1 < GRID_X) && (y_x_plus1_free > -1)){ // space below left and right is free -> fail randomly
                        m_SandCornLifeTime[y][x] += ts.asMilliseconds();
                        int random = rand() % 2;
                        if (random == 0){ // move left
                            m_Grid[y_x_minus1_free][x-1] = true;
                            m_SandCornLifeTime[y_x_minus1_free][x-1] = m_SandCornLifeTimeBuffer[y][x] + ts.asMilliseconds();
                        }
                        if (random == 1){ // move right
                            m_Grid[y_x_plus1_free][x+1] = true;
                            m_SandCornLifeTime[y_x_plus1_free][x+1] = m_SandCornLifeTimeBuffer[y][x] + ts.asMilliseconds();
                        }
                        m_Grid[y][x]   = false;
                        m_SandCornLifeTime[y][x] = 0.0f;
                    }
                    else if ((x-1 >= 0) && (y_x_minus1_free > -1)){
                        m_SandCornLifeTime[y][x] += ts.asMilliseconds();
                        m_Grid[y_x_minus1_free][x-1] = true;
                        m_SandCornLifeTime[y_x_minus1_free][x-1] = m_SandCornLifeTimeBuffer[y][x] + ts.asMilliseconds();
                        m_Grid[y][x]   = false;
                        m_SandCornLifeTime[y][x] = 0.0f;
                    }
                    else if ((x+1 < GRID_X) && (y_x_plus1_free > -1)){
                        m_SandCornLifeTime[y][x] += ts.asMilliseconds();
                        m_Grid[y_x_plus1_free][x+1] = true;
                        m_SandCornLifeTime[y_x_plus1_free][x+1] = m_SandCornLifeTimeBuffer[y][x] + ts.asMilliseconds();
                        m_Grid[y][x]   = false;
                        m_SandCornLifeTime[y][x] = 0.0f;
                    }
                }
            }
        }
    }

    void UpdateObstacles(sf::RenderWindow& window, const sf::Time& ts){
        //get current mouse position
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        // only create new obstacle if mouse position is on screen
        if (MouseOnScreen(mousePos) && (sf::Mouse::isButtonPressed(sf::Mouse::Right))){
            // scale screen position to grid position
            m_GridObstacles[mousePos.y / (SCAL_FACTOR_Y)][mousePos.x / (SCAL_FACTOR_X)] = true;
        }
    }
    void DrawSand(sf::RenderWindow& window){

        sf::CircleShape circle;
        circle.setRadius(SAND_RAD);

        for (int y = 0 ; y < GRID_Y; y++){
            for (int x = 0 ; x < GRID_X; x++){
                if (m_Grid[y][x]){
                    circle.setFillColor(sf::Color(rand() % 255,rand() % 255,rand() % 255));
                    // calculate position on screen
                    if (x * SCAL_FACTOR_X > WINDOW_X)
                        std::cout << "BIGGER" << std::endl;

                    circle.setPosition(x * SCAL_FACTOR_X, y * SCAL_FACTOR_Y);
                    window.draw(circle);
                }
                m_GridBuffer[y][x] = m_Grid[y][x];
                m_SandCornLifeTimeBuffer[y][x] = m_SandCornLifeTime[y][x];
            }
        }
    }

    void DrawObstacles(sf::RenderWindow& window){
        sf::RectangleShape rectangle;
        rectangle.setSize(sf::Vector2f(SAND_RAD * 2, SAND_RAD * 2));
        for (int y = 0 ; y < GRID_Y; y++){
            for (int x = 0 ; x < GRID_X; x++){
                if (m_GridObstacles[y][x]){
                    rectangle.setFillColor(sf::Color(sf::Color::White));
                    // calculate position on screen
                    if (x * SCAL_FACTOR_X > WINDOW_X)
                        std::cout << "BIGGER" << std::endl;

                    rectangle.setPosition(x * SCAL_FACTOR_X, y * SCAL_FACTOR_Y);
                    window.draw(rectangle);
                }

            }
        }
    }

    bool MouseOnScreen(sf::Vector2i mousePos){
        return (mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < WINDOW_X && mousePos.y < WINDOW_Y);
    }

    //reset world
    void Reset(){
        for (int x = 0; x < GRID_X; x++){
            for (int y = 0; y < GRID_Y; y++){
                m_Grid[y][x] = false;
                m_GridBuffer[y][x] = false;
            }
        }
    }
private:

    int GetNextFreeGridPos(int y_startIdx, int y_endIdx, int x){
        int freePos = -1;
        for (int y = y_startIdx; y <= y_endIdx; y++){
            if (m_GridBuffer[y][x] == false && m_GridObstacles[y][x] == false){
                freePos = y;
            } else {
                break;
            }
        }
        return freePos;
    }
private:
    bool m_Grid[GRID_Y][GRID_X] = {false};
    bool m_GridBuffer[GRID_Y][GRID_X] = {false};
    bool m_GridObstacles[GRID_Y][GRID_X] = {false};
    float m_SandCornLifeTime[GRID_Y][GRID_X] = {0.0f};
    float m_SandCornLifeTimeBuffer[GRID_Y][GRID_X] = {0.0f};

};


int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "Falling Sand!");
    World world;
    window.setFramerateLimit(60);
    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        world.Update(window, elapsed);

        window.clear();
        world.DrawSand(window);
        world.DrawObstacles(window);
        window.display();

    }

    return 0;
}