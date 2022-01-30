#include "Resources.hpp"

namespace jui
{
    ResourceHandler<std::string, sf::Font> Fonts::s_fontTable;

    bool Fonts::load(const std::string& p_name, const std::string& p_filePath)
    {
        return s_fontTable.load(p_name, p_filePath);
    }
    sf::Font& Fonts::get(const std::string& p_name)
    {
        return s_fontTable.get(p_name);
    }

    Cursor::CursorData::CursorData()
    {
        for (int i = 0; i < 13; i++)
        {
            if(!system_cursors[i].loadFromSystem((sf::Cursor::Type)i))
            {
                std::cout << "Failed to load system cursor (" << i << ")!" << std::endl;
            }
        }
    }

    void Cursor::CursorData::setCusor(sf::Cursor::Type p_type, sf::RenderWindow& p_window)
    {
        p_window.setMouseCursor(system_cursors[(int)p_type]);
    }

    Cursor::CursorData Cursor::s_data;

    void Cursor::setCusor(sf::Cursor::Type p_type, sf::RenderWindow& p_window)
    {
        s_data.setCusor(p_type, p_window);
    }

    Icon::Icon(const sf::Texture* p_texture)
        : texture(p_texture)
    {
        shape.setTexture(*texture);
    }

    void Icon::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(shape);
    }

    void Icon::setTexture(const sf::Texture* p_texture)
    {
        texture = p_texture;
    }

    void Icon::setPosition(const sf::Vector2f& p_position)
    {
        shape.setPosition(p_position);
    }

    const sf::Vector2f& Icon::getPosition()
    {
        return shape.getPosition();
    }

    void Icon::setSize(const sf::Vector2f& p_size)
    {
        shape.setScale(sf::Vector2f(p_size.x / getSize().x, p_size.y / getSize().y));
    }

    sf::Vector2f Icon::getSize()
    {
        if(texture == nullptr) return sf::Vector2f();
        return sf::Vector2f(texture->getSize().x * shape.getScale().x, texture->getSize().y * shape.getScale().y);
    }  
};