# JUI-Library
JUI ist a graphical user interface (GUI) library written in C++ using SFML. Therefore it should go along nicely with other SFML projects. It implements UI basics like clipping, updating and layers in a system of widgets that are controlled by a canvas. The default UI elements are TextField, Button, ScrollList, DropDownList, InputField, more, but it is also extensible.
Furthermore it provides quite a few utility functions as well as classes for handeling resources such as Textures, Fonts, Cursors.

# How to use

Just include these 6 files into your project and include the headers into your project files as needed. When compiling the executable you also have to provide a working SFML build. 

# Example
This code generates a simple application with a button that can be pressed. 

```html
#include <iostream>

#include "JUI.hpp"
#include "Utils.hpp"
#include "Resources.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 900), "JUI works");

    jui::Fonts::load("Arial", "arial.ttf");

    jui::Canvas canvas(window);

    jui::Button button(canvas, jui::Fonts::get("Arial"));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            canvas.updateAllEvents(event);
        }

        window.clear();
        canvas.drawAll();
        canvas.updateAllLogic();
        window.display();
    }
}
```
![ButtonExample](https://github.com/yoshy3904/JUI-Library/blob/main/example/button.PNG)
