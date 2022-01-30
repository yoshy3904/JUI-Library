# JUI-Library
JUI ist a graphical user interface (GUI) library written in C++ using SFML. It implements UI basics like clipping, updating and layers in a system of widgets that are controlled by a canvas. The default UI elements are TextField, Button, ScrollList, DropDownList, InputField, more, but it is also extensible.
Furthermore it provides quite a few utility functions as well as classes for handeling resources such as Textures, Fonts, Cursors.

# Building the source code

This is not that difficult since the library includes only 3 .cpp files with their respective header files. A working SFML build has to be provided.

`cmake_minimum_required(VERSION 3.15.2)`
