#pragma once
#include <SFML/Graphics.hpp>
#include <sstream>

namespace jui
{
    /*
    @return Get the current mouse position relative to the window. If no view is provided the view currently used by the window is taken. 
    @param window The window used.
    @param view The view whose local space the mouse position should be retrieved from.
    */
    sf::Vector2f getMousePosition(sf::RenderWindow& window, const sf::View* view = nullptr);
    /*
    @return Get the current mouse position relative to a point. If no point is specified the window's upper left corner is used. 
    If no view is provided the view currently used by the window is used. 
    @param window The window used.
    @param view The view whose local space the mouse position should be retrieved from.
    @param relative_to The point to which the mouse position should be relative to. 
    */
    sf::Vector2f getMousePosition(sf::RenderWindow& window, const sf::Vector2f& relative_to, const sf::View* view = nullptr);

    /*
    @return Check whether the mouse is over a circle constructed from a center and radius.
    */
    bool mouseOverCircle(const sf::Vector2f& center, float radius, sf::RenderWindow& window, sf::View* view = nullptr);

    /*
    @return Check whether the mouse is over an axis-aligned rectangle.
    */
    bool mouseOverRectangle(const sf::FloatRect& rectangle, sf::RenderWindow& window, sf::View* view = nullptr);

    /*
    Check if the mouse is over the edges of a rectangle.
    @return 1 = top, 2 = right, 3 = bottom, 4 = left, 0 (false) = no collision
    */
    int mouseOverRectEdge(const sf::FloatRect& rectangle, float edge_thickness, sf::RenderWindow& window, sf::View* view = nullptr);

    /*
    Takes a position and returns the index of the nearest character in the sf::Text object. It is the inverse operation of the sfml function sf::Text::findCharacterPos().
    */
    int getCharIndexByPosition(sf::Text& text, sf::Vector2f position);

    /*
    Transform some value into a sf::String with precision.
    */
    template <typename T>
    sf::String toString(const T p_value, const int p_digits = 6)
    {
        std::ostringstream out;
        out.precision(p_digits);
        out << std::fixed << p_value;
        sf::String replaced_string(out.str());
        replaced_string.replace(".", ",");
        return replaced_string;
    }

    sf::String toBinaryString(unsigned int p_value);
    sf::String toHexadecimalString(unsigned int p_value, bool p_uppercase = true);

    /*
    Split a string by some character.
    */
    std::vector<sf::String> splitStringByChar(const sf::String& str, const wchar_t& c);
    
    /*
    Transform a string into a decimal number. Dots as well as commas are valid. Invalid strings return 0.
    */
    float stringToFloat(const sf::String& p_string);

    /* 
    Transform a string into a integer. Invalid strings return 0.
    */
    int stringToInt(const sf::String& p_string);

    sf::String toUppercase(const sf::String& p_string);
    sf::Uint32 toUppercase(sf::Uint32 p_char);

    unsigned int hexadecimalToInt(sf::String p_string);
    unsigned int binaryToInt(sf::String p_string);

    template<typename T>
    T clampValue(const T& p_value, const T& p_min, const T& p_max)
    {
        T modified_value = p_value;
        if(modified_value < p_min) modified_value = p_min;
        else if (modified_value > p_max) modified_value = p_max;
        return modified_value;
    }

    void scaleImage(const sf::Image& p_source_image, sf::Image& p_destination_image, const sf::Vector2i& p_size);

    void scaleBorderedImage(const sf::Image& p_source_image, sf::Image& p_destination_image, const sf::Vector2f& p_size, int p_border_left, int p_border_right, int p_border_top, int p_border_bottom);
};