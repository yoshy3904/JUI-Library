cmake_minimum_required(VERSION 3.15.2)
set(CMAKE_CXX_STANDARD 17)

project(JUI)
add_library(${PROJECT_NAME} STATIC src/JUI.cpp src/Resources.cpp src/Utils.cpp)

# Define the libraries to be used.
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "...")
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
target_link_libraries(JUI PUBLIC sfml-graphics)

# Set directories
target_include_directories(JUI PUBLIC "include/")
