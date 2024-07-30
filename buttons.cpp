#include <SFML/Graphics.hpp>
#include <iostream>
class Button : public sf::RectangleShape
{
    int _button_size = 50, _x_number = 4, _grid_size = 50, _spacing = 2;
public:
    Button()
    {
        this->setSize(sf::Vector2f(_button_size, _button_size));
    }
    Button(int sizex, int sizey, int position)
    {
        this->set(sizex, sizey, position);
    };
    void set(int sizex, int sizey, int position)
    {
        this->setSize(sf::Vector2f(_button_size, _button_size));
        int x, y;
        x = position % _x_number;
        y = position / _x_number;
        this->setPosition((sizex + 1) * _grid_size + _grid_size * x * _spacing, _grid_size + _grid_size * y * _spacing);
    };

    bool is_inside_me(sf::RenderWindow &window)
    {
        auto mouse_pos = sf::Mouse::getPosition(window); // Mouse position relative to the window
        auto translated_pos = window.mapPixelToCoords(mouse_pos); // Mouse position translated into world coordinates
        if(this->getGlobalBounds().contains(translated_pos)) // Rectangle-contains-point check
            return true;
        return false;
    }

};



