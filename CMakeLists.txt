cmake_minimum_required(VERSION 3.24)
project(minesweeper)

set(CMAKE_CXX_STANDARD 17)

# Add the executable and source files
add_executable(minesweeper
        main.cpp
        welcomeWindow.cpp
        board.cpp
        welcomeWindow.h
        board.h)

# Set SFML directories and find SFML components
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

# Link SFML libraries to the executable
target_link_libraries(minesweeper PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)
