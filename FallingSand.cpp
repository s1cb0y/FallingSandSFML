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
        
        sf::Event event;        
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        //get current mouse position
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);    
        // only create new sandcorn of mouse position is on screen
        if (MouseOnScreen(mousePos)){                        
            // scale screen position to grid position
            m_Grid[mousePos.y / (SCAL_FACTOR_Y)][mousePos.x / (SCAL_FACTOR_X)] = true;
        }        
        
        // Update all sandcorn
        for (int x = 0 ; x < GRID_X-2; x++){
            for (int y = 0 ; y < GRID_Y-2; y++){
                if (m_GridBuffer[y][x] == true){
                    if (m_GridBuffer[y+1][x] == false){
                        m_Grid[y+1][x] = true;
                        m_Grid[y][x]   = false;
                    }
                    else if (m_GridBuffer[y+1][x-1] == false){
                        m_Grid[y+1][x-1] = true;
                        m_Grid[y][x]   = false;
                    }
                    else if (m_GridBuffer[y+1][x+1] == false){
                        m_Grid[y+1][x+1] = true;
                        m_Grid[y][x]   = false;
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
                    // calculate position on screen            
                    m_Shape.setPosition(x * SCAL_FACTOR_X, y * SCAL_FACTOR_Y);
                    window.draw(m_Shape);
                }
                m_GridBuffer[y][x] = m_Grid[y][x];
            }
        }
    }

    bool MouseOnScreen(sf::Vector2i mousePos){
        return (mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < WINDOW_X && mousePos.y < WINDOW_Y);
    }

private:         
    bool m_Grid[GRID_Y][GRID_X] = {false};
    bool m_GridBuffer[GRID_Y][GRID_X] = {false};
    
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