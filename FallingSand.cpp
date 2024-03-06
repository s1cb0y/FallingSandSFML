#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>

#define SAND_RAD 4
#define GRID_X 100
#define GRID_Y 100
#define WINDOW_X 800
#define WINDOW_Y 800
#define SCAL_FACTOR_X 8
#define SCAL_FACTOR_Y 8

class World{
public:
    World() {        
    }
    ~World(){}

    void Update(sf::RenderWindow& window, const sf::Time& ts){
        
        //get current mouse position
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        sf::Event event;        
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed){
                m_CreateSand = true;
            }
            if (event.type == sf::Event::MouseButtonReleased){
                m_CreateSand = false;
            }
        }
        
        // only create new sandcorn of mouse position is on screen
        if (MouseOnScreen(mousePos) && m_CreateSand){
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
    void DrawSand(sf::RenderWindow& window){
                
        sf::CircleShape m_Shape; 
        m_Shape.setRadius(SAND_RAD);
        
        for (int y = 0 ; y < GRID_Y; y++){
            for (int x = 0 ; x < GRID_X; x++){
                if (m_Grid[y][x]){
                    m_Shape.setFillColor(sf::Color(rand() % 255,rand() % 255,rand() % 255));
                    //m_Shape.setFillColor(sf::Color::Red);
                    // calculate position on screen
                    if (x * SCAL_FACTOR_X > WINDOW_X)
                        std::cout << "BIGGER" << std::endl;

                    m_Shape.setPosition(x * SCAL_FACTOR_X, y * SCAL_FACTOR_Y);
                    window.draw(m_Shape);
                }
                m_GridBuffer[y][x] = m_Grid[y][x];
                m_SandCornLifeTimeBuffer[y][x] = m_SandCornLifeTime[y][x];
            }
        }
    }

    bool MouseOnScreen(sf::Vector2i mousePos){
        return (mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < WINDOW_X && mousePos.y < WINDOW_Y);
    }
private:
    int GetNextFreeGridPos(int y_startIdx, int y_endIdx, int x){
        int freePos = -1;
        for (int y = y_startIdx; y <= y_endIdx; y++){
            if (m_GridBuffer[y][x] == false){
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
    float m_SandCornLifeTime[GRID_Y][GRID_X] = {0.0f};
    float m_SandCornLifeTimeBuffer[GRID_Y][GRID_X] = {0.0f};
    bool m_CreateSand = false;
    
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
        window.display();
     
    }

    return 0;
}