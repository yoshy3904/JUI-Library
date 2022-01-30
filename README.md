# JUI-Library
JUI ist a graphical user interface (GUI) library written in C++ using SFML. It implements UI basics like clipping, updating and layers in a system of widgets that are controlled by a canvas. The default UI elements are TextField, Button, ScrollList, DropDownList, InputField, more, but it is also extensible.
Furthermore it provides quite a few utility functions as well as classes for handeling resources such as Textures, Fonts, Cursors.

# Building the source code

This is not that difficult since the library includes only 3 .cpp files with their respective header files. A working SFML build has to be provided.

```html
cmake_minimum_required(VERSION 3.15.2)
set(CMAKE_CXX_STANDARD 17)

project(JUI)
add_library(${PROJECT_NAME} STATIC src/JUI.cpp src/Resources.cpp src/Utils.cpp)

# Define the libraries to be used.
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR <Location of SFML build here>)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
target_link_libraries(JUI PUBLIC sfml-graphics)

# Set directories
target_include_directories(JUI PUBLIC "include/")
```
