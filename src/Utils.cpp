#include "Utils.hpp"
#include <iostream>

namespace jui
{
    sf::Vector2f getMousePosition(sf::RenderWindow& window, const sf::View* view) 
    {
        sf::Vector2f mouse_position;
        if(view == nullptr)
        {
            // "mapPixelToCoords" is used to retrieve the correct coordinates in world space even if the view is distorted.
            mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }
        else
        {
            mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window), *view);
        }
        return mouse_position;
    }

    sf::Vector2f getMousePosition(sf::RenderWindow& window, const sf::Vector2f& relative_to, const sf::View* view) 
    {
        sf::Vector2f mouse_position;
        if(view == nullptr)
        {
            // "mapPixelToCoords" is used to retrieve the correct coordinates in world space even if the view is distorted.
            mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        }
        else
        {
            mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window), *view);
        }
        mouse_position -= relative_to;
        return mouse_position;
    }

    bool mouseOverCircle(const sf::Vector2f& center, float radius, sf::RenderWindow& window, sf::View* view) 
    {
        sf::Vector2f mousePosition = getMousePosition(window, view);
        sf::Vector2f distanceVector = mousePosition - sf::Vector2f(center.x + radius, center.y + radius);
        float distance = std::sqrt(distanceVector.x * distanceVector.x + distanceVector.y * distanceVector.y);

        return distance < radius;
    }

    bool mouseOverRectangle(const sf::FloatRect& rectangle, sf::RenderWindow& window, sf::View* view) 
    {
        sf::Vector2f mousePosition = getMousePosition(window, view);
        return rectangle.contains(mousePosition);
    }

    int mouseOverRectEdge(const sf::FloatRect& rectangle, float edge_thickness, sf::RenderWindow& window, sf::View* view) 
    {
        if(mouseOverRectangle(sf::FloatRect(rectangle.left, rectangle.top, rectangle.width, edge_thickness), window, view))
        {
            return 1;
        }
        if(mouseOverRectangle(sf::FloatRect(rectangle.left + rectangle.width - edge_thickness, rectangle.top, edge_thickness, rectangle.height), window, view))
        {
            return 2;
        }
        if(mouseOverRectangle(sf::FloatRect(rectangle.left, rectangle.top + rectangle.height - edge_thickness, rectangle.width, edge_thickness), window, view))
        {
            return 3;
        }
        if(mouseOverRectangle(sf::FloatRect(rectangle.left, rectangle.top, edge_thickness, rectangle.height), window, view))
        {
            return 4;
        }

        return false;
    }

    int getCharIndexByPosition(sf::Text& text, sf::Vector2f position) 
    {
        std::vector<float> distances;

        for (size_t i = 0; i < text.getString().getSize() + 1; i++)
        {
            sf::Vector2f gPos = text.findCharacterPos(i);
            gPos.y += text.getCharacterSize() / 2.f;
            sf::Vector2f diff = sf::Vector2f(position.x - gPos.x, position.y - gPos.y);
            distances.push_back(sqrtf(diff.x * diff.x + diff.y * diff.y));
        }
        return std::min_element(distances.begin(), distances.end()) - distances.begin();
    }

    std::vector<sf::String> splitStringByChar(const sf::String& str, const wchar_t& c)
    {
        std::wstringstream strstream;
        strstream << str.toWideString();
        std::vector<sf::String> segmentList;
        std::wstring segment;

        while (std::getline(strstream, segment, c))
        {
            segmentList.push_back(segment);
        }

        return segmentList;
    }

    float stringToFloat(const sf::String& p_string)
    {
        if(p_string.getSize() == 0) return 0.f;
        if(p_string.toAnsiString().find_first_not_of("+-0123456789.,") != std::string::npos) return 0.f;

        // Replace dots with comma.
        sf::String replaced_string(p_string);
        replaced_string.replace(".", ",");
        bool negative = replaced_string.find("-") != sf::String::InvalidPos;
        replaced_string.replace("-", "");
        replaced_string.replace("+", "");

        // Split number into two parts.
        std::vector<sf::String> number_parts = splitStringByChar(replaced_string, ',');
        float sum = 0.f;
        float factor = std::pow(10.f, number_parts[0].getSize() - 1.f);
        float value = 0.f;
        if(number_parts.size() < 1) return 0.f;
        // Add numbers before the comma to sum.
        for (int i = 0; i < number_parts[0].getSize(); i++)
        {
            value = number_parts[0][i] - 0x30;
            sum += factor * value;
            factor /= 10.f;
        }
        // Add numbers after the comma to sum.
        if(number_parts.size() >= 2)
        {
            factor = 0.1f;
            for (int i = 0; i < number_parts[1].getSize(); i++)
            {
                value = number_parts[1][i] - 0x30;
                sum += factor * value;
                factor /= 10.f;
            }
        }

        if(negative) return -sum;
        return sum;
    }

    int stringToInt(const sf::String& p_string)
    {
        if(p_string.getSize() == 0) return 0.f;
        if(p_string.toAnsiString().find_first_not_of("+-0123456789.,") != std::string::npos) return 0.f;

        // Replace dots with comma.
        sf::String replaced_string(p_string);
        replaced_string.replace(".", ",");
        bool negative = replaced_string.find("-") != sf::String::InvalidPos;
        replaced_string.replace("-", "");
        replaced_string.replace("+", "");

        // Discard everthing after a comma.
        std::vector<sf::String> number_parts = splitStringByChar(replaced_string, ',');
        if(number_parts.size() < 1) return 0.f;
        sf::String splitted_string = number_parts[0];

        // Split number into two parts.
        float sum = 0.f;
        float factor = std::pow(10.f, splitted_string.getSize() - 1.f);
        float value = 0.f;
        // Add numbers before the comma to sum.
        for (int i = 0; i < splitted_string.getSize(); i++)
        {
            value = splitted_string[i] - 0x30;
            sum += factor * value;
            factor /= 10.f;
        }
        
        if(negative) return -sum;
        return sum;
    }

    sf::String toUppercase(const sf::String& p_string)
    {
        sf::String uppercase_string;
        for (int i = 0; i < p_string.getSize(); i++)
        {
            // Add transformed char to new string.
            uppercase_string += toUppercase(p_string[i]);
        }
        return uppercase_string;
    }
    sf::Uint32 toUppercase(sf::Uint32 p_char)
    {
        // Find conversion to uppercase.
        if((p_char >= 0x61 && p_char <= 0x7a) || // ASCII letters.
            (p_char >= 0xe0 && p_char <= 0xfe && p_char != 0xf7)) // Extendend ANSI letters.
        {
            p_char -= 0x20;
        }

        return p_char;
    }

    unsigned int hexadecimalToInt(sf::String p_string)
    {
        // Check if p_string is valid.
        p_string.replace(" ", "");
        int found = p_string.find("0x");
        if(found != sf::String::InvalidPos) p_string.erase(found, 2);
        p_string = toUppercase(p_string);
        if(p_string.toAnsiString().find_first_not_of("0123456789ABCDEF") != std::string::npos) return 0;        

        // Calculate integer value.
        unsigned int sum = 0;
        unsigned int factor = std::pow(16, p_string.getSize() - 1);
        for (int i = 0; i < p_string.getSize(); i++)
        {
            unsigned int value = 0;
            if(p_string[i] >= 0x30 && p_string[i] <= 0x39) value = p_string[i] - 0x30;
            else if(p_string[i] >= 0x41 && p_string[i] <= 0x46) value = p_string[i] - 0x37;
            sum += factor * value;
            factor /= 16;
        }
        return sum;
    }

    unsigned int binaryToInt(sf::String p_string)
    {
        // Check if p_string is valid.
        p_string.replace(" ", "");
        int found = p_string.find("0b"); 
        if(found != sf::String::InvalidPos) p_string.erase(found, 2);
        if(p_string.toAnsiString().find_first_not_of("01") != std::string::npos) return 0;

        // Calculate integer value.
        unsigned int sum = 0;
        unsigned int factor = std::pow(2, p_string.getSize() - 1);
        for (int i = 0; i < p_string.getSize(); i++)
        {
            sum += p_string[i] == '1' ? factor : 0;
            factor /= 2;
        }
        
        return sum;
    }

    sf::String toBinaryString(unsigned int p_value)
    {
        sf::String string;

        while(p_value != 0)
        {
            char digit = '0' + (p_value & 1);
            string += digit;
            
            p_value >>= 1;
        }

        sf::String reversed_string;
        for (int i = string.getSize() - 1; i >= 0; i--)
        {
            reversed_string += string[i];
        }
        reversed_string.insert(0, "0b");
        
        return reversed_string;
    }

    sf::String toHexadecimalString(unsigned int p_value, bool p_uppercase)
    {
        sf::String string;
        int sum = 0;
        int exponent = 1;
        while(sum != p_value)
        {
            int digit = (p_value % (int)std::pow(16, exponent)) / std::pow(16, exponent - 1);
            if(digit <= 9)
            {
                string += (char)('0' + digit);
            }
            else
            {
                string += (char)(p_uppercase ? 'A' + (digit - 10) : 'a' + (digit - 10));
            }
            sum += digit * std::pow(16, exponent - 1);
            exponent++;
        }

        sf::String reversed_string;
        for (int i = string.getSize() - 1; i >= 0; i--)
        {
            reversed_string += string[i];
        }
        reversed_string.insert(0, "0x");

        return reversed_string;
    }

    void scaleImage(const sf::Image& p_source_image, sf::Image& p_destination_image, const sf::Vector2i& p_size)
    {
        if(p_size.x == 0 || p_size.y == 0) return;
        p_destination_image.create(std::abs(p_size.x), std::abs(p_size.y));
        for (int y = 0; y < p_destination_image.getSize().y; y++)
        {
            for (int x = 0; x < p_destination_image.getSize().x; x++)
            {
                sf::Vector2f relative_position = sf::Vector2f(x / (float)p_destination_image.getSize().x, y / (float)p_destination_image.getSize().y);
                p_destination_image.setPixel(x, y, p_source_image.getPixel(relative_position.x * (float)p_source_image.getSize().x, relative_position.y * (float)p_source_image.getSize().y));   
            }
        }
    }

    /*
    Scales a image but preserves the borders, meaning they wont be scaled normally. The corners will always keep the same size as the original image. 
    Edges will be scalled up only horizontally or vertically. The resulting middle part will be scaled up to match the new size. (8 image.copy() calls, 2 scaleImage() calls)
    @param p_source_image The image that should get scaled up.
    @param p_destination_image Reference to the image that will contain the scaled image. The function automaticaly resizes the image.
    @param p_size The new size.
    @param p_border_* The size of a border in pixels.
    */
    void scaleBorderedImage(const sf::Image& p_source_image, sf::Image& p_destination_image, const sf::Vector2f& p_size, int p_border_left, int p_border_right, int p_border_top, int p_border_bottom)
    {
        p_destination_image.create(p_size.x, p_size.y);
        sf::Image image, resized_image, horizontal_image;
        horizontal_image.create(p_destination_image.getSize().x, p_source_image.getSize().y);

        // Copy left and right edges to horizontal_image.
        horizontal_image.copy(p_source_image, 0, 0, sf::IntRect(0, 0, p_border_left, p_source_image.getSize().y));
        horizontal_image.copy(p_source_image, p_destination_image.getSize().x - p_border_right, 0, sf::IntRect(p_source_image.getSize().x - p_border_right, 0, p_border_left, p_source_image.getSize().y));

        // Copy middle part and stretch it horizontally to fit the horizontal_image.
        image.create(p_source_image.getSize().x - p_border_left - p_border_right, p_source_image.getSize().y);
        image.copy(p_source_image, 0, 0, sf::IntRect(p_border_left, 0, image.getSize().x, image.getSize().y));
        scaleImage(image, resized_image, sf::Vector2i(p_destination_image.getSize().x - p_border_left - p_border_right, p_source_image.getSize().y));
        horizontal_image.copy(resized_image, p_border_left, 0, sf::IntRect());

        // Coppy middle part and stretch it vertically to fit the destination image.
        image.create(p_destination_image.getSize().x, p_source_image.getSize().y - p_border_top - p_border_bottom);
        image.copy(horizontal_image, 0, 0, sf::IntRect(0, p_border_top, image.getSize().x, image.getSize().y));
        scaleImage(image, resized_image, sf::Vector2i(p_destination_image.getSize().x, p_destination_image.getSize().y - p_border_top - p_border_bottom));
        p_destination_image.copy(resized_image, 0, p_border_top, sf::IntRect());

        // Copy top and bottom edge to destination.
        p_destination_image.copy(horizontal_image, 0, 0, sf::IntRect(0, 0, p_destination_image.getSize().x, p_border_top));
        p_destination_image.copy(horizontal_image, 0, p_destination_image.getSize().y - p_border_bottom, sf::IntRect(0, p_source_image.getSize().y - p_border_bottom, p_destination_image.getSize().x, p_border_bottom));
    }
};

