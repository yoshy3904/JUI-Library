# JUI-Library
JUI is a graphical user interface (GUI) library written in C++ using SFML. Therefore it should go along nicely with other SFML projects. It implements UI basics like clipping, updating and layers in a system of widgets that are controlled by a canvas. The default UI elements are TextField, Button, ScrollList, DropDownList, InputField, more, but it is also extensible.
Furthermore it provides quite a few utility functions as well as classes for handeling resources such as Textures, Fonts, Cursors.

# How to use

Just include these 2 files into your project and include the header into your project. When compiling the executable you also have to provide a working SFML build. 

# Example
This code generates a simple application with a button that can be pressed. 

```html
#include "jui.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1366, 768), "JUI works!");
    window.setFramerateLimit(60);

    jui::Fonts::load("Arial", "arial.ttf");

    jui::ResourceHandler<sf::String, sf::Texture> resourceHandler;
    resourceHandler.load("JUI_Icon", "jui-icon.png");
    resourceHandler.load("Button", "blue_shiny_button.png");
    resourceHandler.load("Diamond", "Diamond.png");

    jui::Canvas canvas(window);

    jui::Button button2(canvas, jui::Fonts::get("Arial"));
    button2.setPosition(sf::Vector2f(100.f, 200.f));
    button2.text.setString("Prints \"Hello\"");
    button2.setOnClickEvent([](){ std::cout << "Hello" << std::endl; });

    jui::InputField inputfield(canvas, jui::Fonts::get("Arial"));
    inputfield.setPosition(sf::Vector2f(100.f, 300.f));
    inputfield.setOnSelectEvent([](){ std::cout << "Selected InputField!" << std::endl; });
    inputfield.setOnDeselectEvent([](){ std::cout << "Deselected InputField!" << std::endl; });
    inputfield.setVisible(false);

    jui::DropDownList dropdownlist(canvas, jui::Fonts::get("Arial"));
    dropdownlist.setList({ "Hallo", "Ich sterbe ", "Welt!", "1234", L"#äüöß", "l", "zeynep"} );
    dropdownlist.setSize(sf::Vector2f(500.f, 40.f));
    dropdownlist.setOnSelectEvent([](){ std::cout << "Selected dropdownlist!" << std::endl; });
    dropdownlist.setOnDeselectEvent([](){ std::cout << "Deselected dropdownlist!" << std::endl; });
    dropdownlist.setPosition(sf::Vector2f(100.f, 100.f));

    dropdownlist.setLayer(3);

    jui::ScrollList scrollList(canvas, jui::Fonts::get("Arial"));
    scrollList.setPosition(sf::Vector2f(700.f, 100.f));

    jui::Button tab_button1(canvas, jui::Fonts::get("Arial"));
    tab_button1.setSize(sf::Vector2f(500.f, 400.f));
    tab_button1.setPosition(sf::Vector2f(100.f, 450.f));
    tab_button1.standard_color = sf::Color::Green;
    jui::Button tab_button2(canvas, jui::Fonts::get("Arial"));
    tab_button2.setPosition(sf::Vector2f(100.f, 490.f));

    scrollList.add(&tab_button1);
    scrollList.listWidgets(jui::Orientation::Vertical);

    scrollList.setLayer(-10);

    jui::TextField text(canvas, jui::Fonts::get("Arial"));
    text.setString(L"I'm a lawyer.");
    text.setHorizontalAlignment(jui::TextField::Alignment::Centered);
    text.setVerticalAlignment(jui::TextField::Alignment::Middle);
    text.setOverflowType(jui::TextField::OverflowType::Overflow);
    text.setPosition(sf::Vector2f(1000.f, 300.f));
    text.setSize(sf::Vector2f(200.f, 100.f));
    text.enableWrapping(true);

    jui::Graphic icon(canvas);
    icon.setTexture(&resourceHandler.get("Diamond"));
    icon.setPreserveAspect(true);

    jui::Widget widget(canvas);
    widget.setPosition(sf::Vector2f(200.f, 350.f));
    widget.setFillColor(sf::Color::Red);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            canvas.updateAllEvents(event);
        }

        window.clear(sf::Color::White);

        canvas.drawAll();
        canvas.updateAllLogic();

        window.display();
    }

    return 0;
}
```
