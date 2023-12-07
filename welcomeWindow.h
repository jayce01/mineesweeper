#ifndef WELCOME_WINDOW_H
#define WELCOME_WINDOW_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace sf;

class WelcomeWindow {
    public:
        WelcomeWindow();
        void run();
        string getEnteredName();
        Font font;
        void setText(Text &text, float x, float y);

    private:
        RenderWindow window;
        RenderWindow gameWindow;
        Text welcomeText;
        Text name;
        Text nameText;
        RectangleShape cursor;

        string nameInput;
        bool cursorVisible;
        Clock cursorClock;
        float screenWidth;
        float screenHeight;
        int rows;
        int cols;

        void handleEvents();
        void update();
        void draw();
        void openGameWindow();
        void getScreenSize();
};

#endif // WELCOME_WINDOW_H