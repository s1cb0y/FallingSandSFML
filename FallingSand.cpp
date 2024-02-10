#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>

#define SAND_RAD 8.0f
#define WINDOW_X 1024
#define WINDOW_Y 768

class SandCorn{
public:    
    SandCorn(sf::Vector2f pos) {
        m_Shape.setRadius(SAND_RAD);
        m_Shape.setPosition(pos);
        m_Shape.setFillColor(sf::Color(rand() % 255,rand() % 255,rand() % 255));
    }
    void Update(const sf::Time& ts){
        if (m_Shape.getPosition().y < WINDOW_Y - SAND_RAD )
            m_Shape.setPosition(m_Shape.getPosition().x, m_Shape.getPosition().y + 0.1);
    }
    
    bool Intersects(const SandCorn& corn){
        return m_Shape.getGlobalBounds().intersects(corn.GetShape().getGlobalBounds());
    }

    bool HitGround() {return m_Shape.getPosition().y >= WINDOW_Y - SAND_RAD; }

    sf::CircleShape GetShape() const { return m_Shape;}    

private:
    sf::CircleShape m_Shape; 
};

class SandBox{
public:
    SandBox(){
        
    }
    ~SandBox(){}

    void Update(sf::RenderWindow& window, const sf::Time& ts){
        
        sf::Event event;        
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
        }
        //get current mouse position
        sf::Vector2i mPos = sf::Mouse::getPosition(window);    
        SandCorn corn(sf::Vector2f(mPos.x - SAND_RAD *0.5f , mPos.y - SAND_RAD * 0.5f));
        if (sandCorns.size() == 0)
            sandCorns.push_back(corn);
        // only create new sandcorn if it does not intersect with last one
        if (!corn.Intersects(sandCorns.back()))
            sandCorns.push_back(corn);

        // update postion of all sand corns
        for (auto& corn : sandCorns){
            // check if sand corn is still falling
            if (!corn.HitGround())
                corn.Update(ts);
        }
    }
    void DrawSand(sf::RenderWindow& window){
        //window.draw(sandCorns.back());
        for (auto& sand : sandCorns){            
            window.draw(sand.GetShape());
        }
    }

private:     
    std::vector<SandCorn> sandCorns;
    bool m_Grid[WINDOW_Y][WINDOW_X];
};


int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_X, WINDOW_Y), "Falling Sand!");
    SandBox sandBox;
    sf::Clock clock;
    while (window.isOpen())
    {    
        sf::Time elapsed = clock.restart();    
        sandBox.Update(window, elapsed);

        window.clear();
        sandBox.DrawSand(window);
        window.display();
    }

    return 0;
}