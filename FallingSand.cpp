#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>

#define SAND_RAD 8.0f
#define WINDOW_X 1024
#define WINDOW_Y 768
#define GRAVITY_ACC 300.0f

class SandCorn{
public:    
    SandCorn(sf::Vector2f pos) {        
        m_Shape.setRadius(SAND_RAD);
        m_Shape.setPosition(pos);
        m_Shape.setFillColor(sf::Color(rand() % 255,rand() % 255,rand() % 255));
        m_InitialPos = pos;        
    }
    void Update(const sf::Time& ts, std::vector<bool>& grid){
        if (!HitGround()) 
            m_LifeTime = m_LifeTime + ts;
            m_Shape.setPosition(m_Shape.getPosition().x, m_InitialPos.y + (0.5 * GRAVITY_ACC * m_LifeTime.asSeconds() *m_LifeTime.asSeconds()));
           // std::cout << "Pos : " << m_Shape.getPosition().x << " , " <<  m_Shape.getPosition().y << std::endl;
    }
    
    bool Intersects(const SandCorn& corn){
        return m_Shape.getGlobalBounds().intersects(corn.GetShape().getGlobalBounds());
    }

    bool HitGround() {return m_Shape.getPosition().y >= WINDOW_Y - 2*SAND_RAD; }

    sf::CircleShape GetShape() const { return m_Shape;}    

private:
    sf::CircleShape m_Shape; 
    sf::Vector2f m_InitialPos;
    sf::Time m_LifeTime;
};

class SandBox{
public:
    SandBox(){        
        m_Grid.resize(WINDOW_Y * WINDOW_X);        
        std::fill(m_Grid.begin(), m_Grid.end(), false);        
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
        
        // update postion of all sand corns
        for (std::vector<SandCorn>::reverse_iterator it = m_SandCorns.rbegin(); it != m_SandCorns.rend(); it++){
            it->Update(ts, m_Grid);
        }

        SandCorn corn(sf::Vector2f(mPos.x - SAND_RAD *0.5f , mPos.y - SAND_RAD * 0.5f));
        if (m_SandCorns.size() == 0)
            m_SandCorns.push_back(corn);
        // only create new sandcorn if it does not intersect with last one
        if (!corn.Intersects(m_SandCorns.back())){
            m_SandCorns.push_back(corn);
        }
            
        
    }
    void DrawSand(sf::RenderWindow& window){
        //window.draw(m_SandCorns.back());
        for (auto& sand : m_SandCorns){            
            window.draw(sand.GetShape());
        }
    }

private:     
    std::vector<SandCorn> m_SandCorns;
    std::vector<bool> m_Grid;
    
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